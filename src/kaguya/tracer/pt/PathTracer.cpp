//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/Config.h>
#include <kaguya/core/light/AreaLight.h>
#include <kaguya/material/Material.h>
#include <kaguya/parallel/RenderPool.h>
#include <kaguya/scene/Shape.h>
#include <kaguya/tracer/pt/PathTracer.h>

#include <iostream>

namespace kaguya {
    namespace tracer {

        using kaguya::core::Interaction;
        using kaguya::material::Material;

        PathTracer::PathTracer() {
            init();
        }

        void PathTracer::init() {
            _samplePerPixel = Config::samplePerPixel;
            _sampleLightProb = Config::sampleLightProb;
            _maxDepth = Config::maxScatterDepth;
            _russianRouletteBounce = Config::russianRouletteDepth;
            _russianRoulette = Config::russianRoulette;
        }


        Spectrum PathTracer::shaderOfProgression(const kaguya::tracer::Ray &ray, kaguya::Scene &scene,
                                                 const Sampler1D *sampler1D,
                                                 MemoryArena &memoryArena) {
            // 最终渲染结果
            Spectrum shaderColor = Spectrum(0);
            // 光线是否是 delta distribution
            bool isSpecular = false;
            // 保存历次反射计算的 (f(p, wi, wo) * G(wo, wi)) / p(wi)
            Spectrum beta = 1.0;
            // 散射光线
            Ray scatterRay = ray;

            // TODO 依然只考虑一个光源的情况
            const std::shared_ptr<Light> light = scene.getLight();
            assert(light != nullptr);

            // 最多进行 _maxDepth 次数弹射
            for (int bounce = 0; bounce < _maxDepth; bounce++) {
                // intersect
                SurfaceInteraction intersection;
                bool isIntersected = scene.intersect(scatterRay, intersection);

                // 此处参考 pbrt 的写法，需要判断 bounce = 0 和 isSpecular 两种特殊情况
                if (bounce == 0 || isSpecular) {
                    if (isIntersected) {
                        assert(intersection.getMaterial() != nullptr);
                        // 如果有交点，则直接从交点上取值
                        if (intersection.getAreaLight() != nullptr) {
                            shaderColor += (intersection.getAreaLight()->lightRadiance(intersection,
                                                                                       -intersection.getDirection()) *
                                            beta);
                        }
                    } else {
                        shaderColor += (beta * background(scatterRay));
                        break;
                    }
                }

                // 终止条件判断
                if (!isIntersected) {
                    // TODO 添加环境光
                    break;
                }

                const Material *material = intersection.getMaterial();
                assert(material != nullptr);

                BSDF *bsdf = intersection.buildBSDF(memoryArena);
                assert(bsdf != nullptr);

                // 判断是否向光源采样
                if (bsdf->allIncludeOf(BXDFType(BSDF_ALL & (~BSDF_SPECULAR)))) {
                    shaderColor += (beta * evaluateDirectLight(scene, intersection, (*bsdf), sampler1D));
                }

                // 计算下一次反射
                Vector3 worldWo = -scatterRay.getDirection();
                Vector3 worldWi = Vector3(0.0);
                // 材质反射类型
                BXDFType bxdfType;
                // p(wi)
                double samplePdf = 0;
                // f(p, wo, wi)
                Spectrum f = bsdf->sampleF(worldWo, &worldWi, &samplePdf, sampler1D, BSDF_ALL, &bxdfType);

                // cosine
                double cosine = std::abs(DOT(intersection.getNormal(), NORMALIZE(worldWi)));
                // 计算 beta
                beta *= (f * cosine / samplePdf);
                // 设置下一次打击光线
                scatterRay = Ray(intersection.getPoint(), NORMALIZE(worldWi));

                isSpecular = (bxdfType & BSDF_SPECULAR) > 0;

                // Terminate path tracing with Russian Roulette
                if (bounce > _russianRouletteBounce) {
                    if (sampler1D->sample() < _russianRoulette) {
                        break;
                    }
                    beta /= (1 - _russianRoulette);
                }
            }
            return shaderColor;
        }

        Spectrum PathTracer::evaluateDirectLight(Scene &scene, const Interaction &eye, const BSDF &bsdf,
                                                 const Sampler1D *sampler1D) {
            // TODO 目前只考虑单个光源
            auto light = scene.getLight();
            // p(wi)
            double lightPdf = 0;
            // light dir
            Vector3 shadowRayDir = Vector3(0.0);
            // visibility tester
            VisibilityTester visibilityTester;
            // 对光源采样采样
            Spectrum luminance = light->sampleFromLight(eye, &shadowRayDir,
                                                        &lightPdf,
                                                        sampler1D,
                                                        &visibilityTester);

            // 排除对光源采样贡献为 0 的情况
            if (lightPdf > EPSILON && !luminance.isBlack()) {
                // 判断 shadowRay 是否击中
                if (visibilityTester.isVisible(scene)) {
                    // cosine
                    double cosine = std::abs(DOT(eye.getNormal(), shadowRayDir));
                    // f(p, wo, wi)
                    Spectrum f = bsdf.f(-eye.getDirection(), shadowRayDir);
                    if (light->isDeltaType()) {
                        // shader spectrum
                        return f * cosine / lightPdf * luminance;
                    } else {
                        // multiple rayImportance sampling
                        double scatterPdf = bsdf.samplePdf(-eye.getDirection(), shadowRayDir);
                        double weight = misWeight(1, lightPdf, 1, scatterPdf);
                        return f * cosine * weight / lightPdf * luminance;
                    }
                }
            }
            // 没有采样到光源
            return Spectrum(0.0);
        }

        bool PathTracer::render() {
            if (_scene != nullptr && _camera != nullptr) {
                int cameraWidth = _camera->getResolutionWidth();
                int cameraHeight = _camera->getResolutionHeight();

                const double sampleWeight = 1.0 / _samplePerPixel;

                auto renderFunc = [this](const int row, const int col, const Sampler1D *sampler1D) -> void {
                    Spectrum ans = {0};

                    const double sampleWeight = 1.0 / _samplePerPixel;
                    // 做 _samplePerPixel 次采样
                    for (int sampleCount = 0; sampleCount < _samplePerPixel; sampleCount++) {
                        MemoryArena arena;
                        auto u = (col + sampler1D->sample()) / double(_camera->getResolutionWidth());
                        auto v = (row + sampler1D->sample()) / double(_camera->getResolutionHeight());

                        Ray sampleRay = _camera->sendRay(u, v);
                        Spectrum shaderColor = shaderOfProgression(sampleRay, *(_scene.get()), sampler1D, arena);

                        ans += shaderColor;
                        arena.clean();
                    }
                    ans *= sampleWeight;
                    _filmPlane->addSpectrum(ans, row, col);
                };
                parallel::RenderPool *pool = parallel::RenderPool::getInstance();
                pool->addRenderTask(renderFunc, cameraWidth, cameraHeight);
                return true;
            } else {
                return false;
            }
        }

        Spectrum PathTracer::background(const Ray &ray) {
            // TODO 临时设计背景色
            return Spectrum(0.0f);

            /*
            if (ray.getDirection().z < 0) {
                return Vector3(0.0f, 0.0f, 0.0f);
            } else {
                return Vector3(1.0f, 1.0f, 1.0f);
            }

            Vector3 dir = NORMALIZE(ray.getDirection());
            double t = 0.5 * (dir.y + 1.0);
            return (1.0 - t) * Vector3(1.0, 1.0, 1.0) + t * Vector3(0.5, 0.7, 1.0);
             */
        }

    }
}