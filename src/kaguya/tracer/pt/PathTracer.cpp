//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/Config.h>
#include <kaguya/core/bssrdf/BSSRDF.h>
#include <kaguya/core/Interaction.h>
#include <kaguya/tracer/pt/PathTracer.h>
#include <kaguya/sampler/SamplerFactory.hpp>
#include <kaguya/parallel/RenderPool.h>

namespace kaguya {
    namespace tracer {

        using kaguya::core::Interaction;
        using kaguya::material::Material;
        using core::bssrdf::BSSRDF;

        PathTracer::PathTracer() {
            init();
        }

        void PathTracer::init() {
            _samplePerPixel = Config::Tracer::sampleNum;
            _sampleLightProb = Config::sampleLightProb;
            _maxDepth = Config::Tracer::maxDepth;
            _russianRouletteBounce = Config::russianRouletteDepth;
            _russianRoulette = Config::russianRoulette;
        }


        Spectrum PathTracer::shaderOfProgression(const Ray &ray, std::shared_ptr<Scene> scene,
                                                 Sampler *sampler,
                                                 MemoryArena &memoryArena) {
            // 最终渲染结果
            Spectrum shaderColor = Spectrum(0);
            // 光线是否是 delta distribution
            bool isSpecularBounce = false;
            // 保存历次反射计算的 (f(p, wi, wo) * G(wo, wi)) / p(wi)
            Spectrum beta = 1.0;
            // 散射光线
            Ray scatterRay = ray;

            // 最多进行 _maxDepth 次数弹射
            for (int bounce = 0; bounce < _maxDepth; bounce++) {
                // intersect
                SurfaceInteraction si;
                bool isIntersected = scene->intersect(scatterRay, si);

                // deal with participating medium
                core::MediumInteraction mi;
                if (scatterRay.getMedium() != nullptr) {
                    beta *= scatterRay.getMedium()->sampleInteraction(scatterRay, sampler, &mi, memoryArena);
                    if (beta.isBlack()) {
                        break;
                    }
                }

                // check if a medium interaction occurs
                if (mi.isValid()) {
                    /* handle medium interaction */
                    // sample1d direct light
                    shaderColor += beta * sampleDirectLight(scene, mi, sampler);

                    /* sample1d new ray */
                    Vector3F worldWo = -scatterRay.getDirection();
                    Vector3F worldWi;
                    mi.getPhaseFunction()->sampleScatter(worldWo, &worldWi, sampler);
                    scatterRay = mi.sendRay(worldWi);
                    isSpecularBounce = false;
                } else {
                    // handle surface interaction
                    // 此处参考 pbrt 的写法，需要判断 bounce = 0 和 isSpecularBounce 两种特殊情况
                    if (bounce == 0 || isSpecularBounce) {
                        if (isIntersected) {
                            // 如果有交点，则直接从交点上取值
                            if (si.getAreaLight() != nullptr) {
                                shaderColor += (si.getAreaLight()->L(
                                        si, -si.direction) * beta);
                            }
                        } else {
                            // 环境光
                            shaderColor += (beta * background(scatterRay));
                            break;
                        }
                    }

                    // 终止条件判断
                    if (!isIntersected) {
                        // TODO 添加环境光
                        break;
                    }

                    // skip medium boundary
                    if (si.getMaterial() == nullptr) {
                        scatterRay = si.sendRay(scatterRay.getDirection());
                        bounce--;
                        continue;
                    }

                    si.buildScatteringFunction(memoryArena);
                    assert(si.bsdf != nullptr);

                    // 判断是否向光源采样
                    if (si.bsdf->allIncludeOf(BXDFType(BSDF_ALL & (~BSDF_SPECULAR)))) {
                        shaderColor += (beta * sampleDirectLight(scene, si, sampler));
                    }

                    // 计算下一次反射
                    Vector3F worldWo = -scatterRay.getDirection();
                    Vector3F worldWi = Vector3F(0.0);
                    // 材质反射类型
                    BXDFType bxdfType;
                    // p(wi)
                    Float samplePdf = 0;
                    // f(p, wo, wi)
                    Spectrum f = si.bsdf->sampleF(worldWo, &worldWi, &samplePdf, sampler, BSDF_ALL, &bxdfType);
                    isSpecularBounce = (bxdfType & BSDF_SPECULAR) > 0;

                    // cosine
                    Float cosine = ABS_DOT(si.rendering.normal, NORMALIZE(worldWi));
                    // 计算 beta
                    beta *= (f * cosine / samplePdf);
                    // 设置下一次打击光线
                    scatterRay = si.sendRay(NORMALIZE(worldWi));

                    if (si.bssrdf != nullptr && (bxdfType & BXDFType::BSDF_TRANSMISSION)) {
                        // Subsurface
                        SurfaceInteraction pi;
                        Float pdf = 0;
                        Spectrum S = si.bssrdf->sampleS(scene, &pi, &pdf, memoryArena, sampler);
                        if (S.isBlack() || pdf == 0) {
                            break;
                        }

                        beta *= S / pdf;
                        shaderColor += beta * sampleDirectLight(scene, pi, sampler);
                        Spectrum f = pi.bsdf->sampleF(pi.wo, &worldWi, &pdf, sampler, BSDF_ALL, &bxdfType);

                        if (f.isBlack() || pdf == 0) {
                            break;
                        }

                        beta *= f * ABS_DOT(worldWi, pi.rendering.normal) / pdf;
                        isSpecularBounce = (bxdfType & BSDF_SPECULAR) > 0;
                        scatterRay = pi.sendRay(NORMALIZE(worldWi));
                    }
                }

                // TODO 透明介质透射考虑 折射率 refraction
                // Terminate path tracing with Russian Roulette
                if (bounce > _russianRouletteBounce) {
                    if (sampler->sample1D() < _russianRoulette) {
                        break;
                    }
                    beta /= (1 - _russianRoulette);
                }
            }

            return shaderColor;
        }

        void PathTracer::render() {
            int width = _camera->getResolutionWidth();
            int height = _camera->getResolutionHeight();

            int nTileX = (width + Config::Parallel::tileSize - 1) / Config::Parallel::tileSize;
            int nTileY = (height + Config::Parallel::tileSize - 1) / Config::Parallel::tileSize;

            std::atomic<int> nFinished(0);

            auto renderFunc = [&](const int idxTileX, const int idxTileY) -> void {

                int startRow = idxTileY * Config::Parallel::tileSize;
                int endRow = std::min(startRow + Config::Parallel::tileSize - 1, height - 1);

                int startCol = idxTileX * Config::Parallel::tileSize;
                int endCol = std::min(startCol + Config::Parallel::tileSize - 1, width - 1);

                Sampler *sampler = sampler::SamplerFactory::newSampler(_samplePerPixel);
                for (int row = startRow; row <= endRow; row++) {
                    for (int col = startCol; col <= endCol; col++) {
                        // set current sampling pixel
                        sampler->forPixel(Point2F(row, col));
//                        sampler->setCurrentSeed(idxTileY * nTileX + idxTileX);

                        Spectrum ans = {0};
                        const Float sampleWeight = 1.0 / _samplePerPixel;
                        // 做 _samplePerPixel 次采样
                        for (int sampleCount = 0; sampleCount < _samplePerPixel; sampleCount++) {
                            MemoryArena arena;
                            auto u = (col + sampler->sample1D()) / Float(_camera->getResolutionWidth());
                            auto v = (row + sampler->sample1D()) / Float(_camera->getResolutionHeight());

                            Ray sampleRay = _camera->sendRay(u, v);
                            Spectrum shaderColor = shaderOfProgression(sampleRay, _scene, sampler, arena);

                            ans += shaderColor;
                            arena.clean();
                            sampler->nextSampleRound();
                        }
                        ans *= sampleWeight;
                        _filmPlane->addSpectrum(ans, row, col);
                    }
                }
                delete sampler;

                nFinished++;
                std::cout << "\r" << float(nFinished) * 100 / (nTileX * nTileY) << " %"
                          << std::flush;
            };

            parallel::parallelFor2D(renderFunc, Point2I(nTileX, nTileY));
        }

        Spectrum PathTracer::background(const Ray &ray) {
            auto lights = _scene->getEnvironmentLights();
            Spectrum ret(0);
            for (Light::Ptr light : lights) {
                ret += light->Le(ray);
            }
            return ret;
        }

        PathTracer::~PathTracer() {}
    }
}