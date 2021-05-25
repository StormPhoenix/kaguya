//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/Config.h>
#include <kaguya/core/bssrdf/BSSRDF.h>
#include <kaguya/core/Interaction.h>
#include <kaguya/core/light/EnvironmentLight.h>
#include <kaguya/core/light/AreaLight.h>
#include <kaguya/tracer/pt/PathTracer.h>
#include <kaguya/tracer/PathRecorder.h>
#include <kaguya/sampler/SamplerFactory.hpp>
#include <kaguya/parallel/RenderPool.h>

#include <atomic>

namespace kaguya {
    namespace tracer {

        using core::AreaLight;
        using core::Interaction;
        using core::EnvironmentLight;
        using material::Material;
        using core::bssrdf::BSSRDF;
        using namespace bsdf;

        PathTracer::PathTracer() {
            init();
        }

        void PathTracer::init() {
            _sampleLightProb = Config::sampleLightProb;
            _russianRouletteBounce = Config::russianRouletteDepth;
            _russianRoulette = Config::russianRoulette;
        }


        Spectrum PathTracer::shaderOfProgression(Point2I pixelPos, int iteration, const Ray &ray,
                                                 std::shared_ptr<Scene> scene, Sampler *sampler,
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
            for (int bounce = 0; bounce < Config::Tracer::maxDepth; bounce++) {
                // intersect
                SurfaceInteraction si;
                bool isIntersected = scene->intersect(scatterRay, si);

                // deal with participating medium
                core::MediumInteraction mi;
                if (scatterRay.getMedium() != nullptr) {
                    beta *= scatterRay.getMedium()->sampleInteraction(scatterRay, sampler, &mi, memoryArena);
                    if (beta.isBlack()) {
                        {
                            // Record intersection
                            if (mi.isValid()) {
                                RECORD_TRACE_PATH(pixelPos.x, pixelPos.y, iteration, mi.point, Medium_Record,
                                                  _camera.get())
                            } else if (isIntersected) {
                                RECORD_TRACE_PATH(pixelPos.x, pixelPos.y, iteration, si.point, Medium_Record,
                                                  _camera.get())
                            } else {
                                RECORD_TRACE_PATH(pixelPos.x, pixelPos.y, iteration, scatterRay.at(20000),
                                                  Environment_Record, _camera.get())
                            }
                        }
                        break;
                    }
                }

                // check if a medium interaction occurs
                if (mi.isValid()) {
                    // handle medium interaction
                    // sample1d direct light
                    shaderColor += beta * sampleDirectLight(scene, mi, sampler);

                    /* sample1d new ray */
                    Vector3F worldWo = -scatterRay.getDirection();
                    Vector3F worldWi;
                    mi.getPhaseFunction()->sampleScatter(worldWo, &worldWi, sampler);
                    scatterRay = mi.sendRay(worldWi);
                    isSpecularBounce = false;
                    RECORD_TRACE_PATH(pixelPos.x, pixelPos.y, iteration, mi.point, Medium_Record, _camera.get())
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
                            // Environment light
                            shaderColor += (beta * estimateEnvironmentLights(scatterRay));
                            // Record infinite position
                            RECORD_TRACE_PATH(pixelPos.x, pixelPos.y, iteration, scatterRay.at(20000),
                                              Environment_Record, _camera.get())
                            break;
                        }
                    }

                    // 终止条件判断
                    if (!isIntersected) {
                        RECORD_TRACE_PATH(pixelPos.x, pixelPos.y, iteration, scatterRay.at(20000),
                                          Environment_Record, _camera.get())
                        break;
                    }

                    // skip medium boundary
                    if (si.getMaterial() == nullptr) {
                        scatterRay = si.sendRay(scatterRay.getDirection());
                        bounce--;
                        continue;
                    }

                    si.buildScatteringFunction(memoryArena);
                    ASSERT(si.bsdf != nullptr, "BSDF can't be nullptr. ");

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
                    RECORD_TRACE_PATH(pixelPos.x, pixelPos.y, iteration, si.point,
                                      isSpecularBounce ? Specular_Record : Others_Record, _camera.get())

                    if (samplePdf == 0 || f.isBlack()) {
                        break;
                    }

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
            int width = Config::Camera::width;
            int height = Config::Camera::height;

            int nTileX = (width + Config::Parallel::tileSize - 1) / Config::Parallel::tileSize;
            int nTileY = (height + Config::Parallel::tileSize - 1) / Config::Parallel::tileSize;

            std::atomic<int> nFinished(0);

            int nIterations = Config::Tracer::sampleNum;
            for (int iter = 0; iter < nIterations; iter++) {
                auto renderFunc = [&](const int idxTileX, const int idxTileY) -> void {
                    int startRow = idxTileY * Config::Parallel::tileSize;
                    int endRow = std::min(startRow + Config::Parallel::tileSize - 1, height - 1);

                    int startCol = idxTileX * Config::Parallel::tileSize;
                    int endCol = std::min(startCol + Config::Parallel::tileSize - 1, width - 1);

                    MemoryArena arena;
                    Sampler *sampler = sampler::SamplerFactory::newSampler(Config::Tracer::sampleNum);
                    for (int row = startRow; row <= endRow; row++) {
                        for (int col = startCol; col <= endCol; col++) {
                            // set current sampling pixel
                            sampler->forPixel(Point2I(row, col));
                            sampler->setSampleIndex(iter);

                            Float pixelX = col + sampler->sample1D();
                            Float pixelY = row + sampler->sample1D();
                            Ray sampleRay = _camera->generateRay(pixelX, pixelY, sampler);
                            Spectrum shaderColor = shaderOfProgression(Point2I(col, row), iter + 1, sampleRay,
                                                                       _scene, sampler, arena);

                            _filmPlane->addSpectrum(shaderColor, row, col);
                            arena.clean();
                        }
                    }
                    delete sampler;
                };
                parallel::parallelFor2D(renderFunc, Point2I(nTileX, nTileY));
                nFinished++;
                std::cout << "\r" << float(nFinished) * 100 / (nIterations) << " %" << std::flush;

                // Write image frequently
                if ((Config::writeFrequency > 0 && (iter + 1) % Config::writeFrequency == 0) ||
                    iter == nIterations - 1) {
                    Float sampleWeight = 1.0 / (iter + 1);
                    std::string suffixSSP;
                    std::stringstream ss;
                    ss << "_SSP" << iter + 1 << "_";
                    ss >> suffixSSP;

                    _filmPlane->writeImage((Config::filenamePrefix + suffixSSP + _scene->getName()).c_str(),
                                           sampleWeight);
                }
            }
            writeImage(Config::filenamePrefix + "_" + _scene->getName(), 1.0 / nIterations);
            std::cout << std::endl << "scene " << _scene->getName() << " completed." << std::endl;
        }

        Spectrum PathTracer::estimateEnvironmentLights(const Ray &ray) {
            auto lights = _scene->getEnvironmentLights();
            Spectrum ret(0);
            for (EnvironmentLight::Ptr light : lights) {
                ret += light->Le(ray);
            }
            return ret;
        }

        PathTracer::~PathTracer() {}
    }
}