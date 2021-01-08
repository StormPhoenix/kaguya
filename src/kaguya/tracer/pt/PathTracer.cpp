//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/Config.h>
#include <kaguya/core/Interaction.h>
#include <kaguya/core/light/AreaLight.h>
#include <kaguya/core/medium/Medium.h>
#include <kaguya/material/Material.h>
#include <kaguya/scene/meta/Shape.h>
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
                                                 Sampler *sampler1D,
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
                SurfaceInteraction si;
                bool isIntersected = scene.intersect(scatterRay, si);

                // deal with participating medium
                core::MediumInteraction mi;
                if (scatterRay.getMedium() != nullptr) {
                    beta *= scatterRay.getMedium()->sampleInteraction(scatterRay, sampler1D, &mi, memoryArena);
                    if (beta.isBlack()) {
                        break;
                    }
                }

                // check if a medium interaction occurs
                if (mi.isValid()) {
                    /* handle medium interaction */
                    // sample1d direct light
                    shaderColor += beta * evaluateDirectLight(scene, mi, sampler1D);

                    /* sample1d new ray */
                    Vector3d worldWo = -scatterRay.getDirection();
                    Vector3d worldWi;
                    mi.getPhaseFunction()->sampleScatter(worldWo, &worldWi, sampler1D);
                    scatterRay = mi.sendRay(worldWi);
                    isSpecular = false;
                } else {
                    // handle surface interaction
                    // 此处参考 pbrt 的写法，需要判断 bounce = 0 和 isSpecular 两种特殊情况
                    if (bounce == 0 || isSpecular) {
                        if (isIntersected) {
                            // 如果有交点，则直接从交点上取值
                            if (si.getAreaLight() != nullptr) {
                                shaderColor += (si.getAreaLight()->lightRadiance(
                                        si, -si.getDirection()) * beta);
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

                    // skip medium boundary
                    if (si.getMaterial() == nullptr) {
                        scatterRay = si.sendRay(scatterRay.getDirection());
                        bounce--;
                        continue;
                    }

                    BSDF *bsdf = si.buildBSDF(memoryArena);
                    assert(bsdf != nullptr);

                    // 判断是否向光源采样
                    if (bsdf->allIncludeOf(BXDFType(BSDF_ALL & (~BSDF_SPECULAR)))) {
                        shaderColor += (beta * evaluateDirectLight(scene, si, sampler1D));
                    }

                    // 计算下一次反射
                    Vector3d worldWo = -scatterRay.getDirection();
                    Vector3d worldWi = Vector3d(0.0);
                    // 材质反射类型
                    BXDFType bxdfType;
                    // p(wi)
                    double samplePdf = 0;
                    // f(p, wo, wi)
                    Spectrum f = bsdf->sampleF(worldWo, &worldWi, &samplePdf, sampler1D, BSDF_ALL, &bxdfType);

                    // cosine
                    double cosine = std::abs(DOT(si.getNormal(), NORMALIZE(worldWi)));
                    // 计算 beta
                    beta *= (f * cosine / samplePdf);
                    // 设置下一次打击光线
                    scatterRay = si.sendRay(NORMALIZE(worldWi));
                    isSpecular = (bxdfType & BSDF_SPECULAR) > 0;
                }

                // TODO 透明介质透射考虑 折射率 refraction

                // Terminate path tracing with Russian Roulette
                if (bounce > _russianRouletteBounce) {
                    if (sampler1D->sample1D() < _russianRoulette) {
                        break;
                    }
                    beta /= (1 - _russianRoulette);
                }
            }
            return shaderColor;
        }

        Spectrum PathTracer::evaluateDirectLight(
                Scene &scene, const Interaction &eye,
                Sampler *sampler1D) {
            // TODO 目前只考虑单个光源
            auto light = scene.getLight();
            // p(wi)
            double lightPdf = 0;
            // light dir
            Vector3d wi = Vector3d(0.0);
            // visibility tester
            VisibilityTester visibilityTester;
            // 对光源采样采样
            Spectrum lumi = light->sampleFromLight(eye, &wi,
                                                   &lightPdf, sampler1D,
                                                   &visibilityTester);

            Spectrum ret(0);

            // 排除对光源采样贡献为 0 的情况
            if (lightPdf > math::EPSILON && !lumi.isBlack()) {
                Spectrum f;
                double scatteringPdf = 0.0f;
                if (eye.isMediumInteraction()) {
                    // handle medium interaction
                    const MediumInteraction &mi = (const MediumInteraction &) eye;
                    assert(mi.getPhaseFunction() != nullptr);

                    scatteringPdf = mi.getPhaseFunction()->scatterPdf(-mi.getDirection(), wi);
                    f = Spectrum(scatteringPdf);
                } else {
                    // handle surface interaction
                    const SurfaceInteraction &si = (const SurfaceInteraction &) eye;
                    assert(si.getBSDF() != nullptr);

                    double cosine = ABS_DOT(eye.getNormal(), wi);
                    scatteringPdf = si.getBSDF()->samplePdf(-eye.getDirection(), wi);
                    f = si.getBSDF()->f(-eye.getDirection(), wi) * cosine;
                }

                if (!f.isBlack()) {
                    lumi *= visibilityTester.transmittance(scene, sampler1D);
                    if (!lumi.isBlack()) {
                        if (light->isDeltaType()) {
                            // shader spectrum
                            ret += f / lightPdf * lumi;
                        } else {
                            // multiple rayImportance sampling
                            double weight = math::misWeight(1, lightPdf, 1, scatteringPdf);
                            ret += f * weight / lightPdf * lumi;
                        }
                    }
                }
            }

            // multiple importance sampling
            if (!light->isDeltaType()) {
                const Vector3d wo = -eye.getDirection();
                Spectrum f(0);
                double scatteringPdf = 0;
                bool sampleSpecular = false;
                if (eye.isMediumInteraction()) {
                    // handle medium interaction
                    const MediumInteraction &mi = (const MediumInteraction &) eye;
                    assert(mi.getPhaseFunction() != nullptr);

                    scatteringPdf = mi.getPhaseFunction()->scatterPdf(wo, wi);
                    f = Spectrum(scatteringPdf);
                } else {
                    // handle surface interaction
                    const SurfaceInteraction &si = (const SurfaceInteraction &) eye;
                    assert(si.getBSDF() != nullptr);

                    BXDFType sampleType;
                    f = si.getBSDF()->sampleF(wo, &wi, &scatteringPdf, sampler1D, BSDF_ALL, &sampleType);
                    f *= ABS_DOT(-si.getDirection(), wi);
                    sampleSpecular = (sampleType & BXDFType::BSDF_SPECULAR) != 0;
                }

                if (!f.isBlack() && scatteringPdf > 0) {
                    double weight = 1.0;
                    if (!sampleSpecular) {
                        // sample1d light pdf
                        lightPdf = light->sampleFromLightPdf(eye, wi);
                        if (lightPdf == 0) {
                            return ret;
                        }
                        weight = math::misWeight(1, scatteringPdf, 1, lightPdf);
                    }

                    // find intersection
                    SurfaceInteraction misSI;
                    Spectrum misTr = 1.0;
                    Ray misRay(eye.getPoint(), NORMALIZE(wi));
                    bool foundIntersection = scene.intersectWithMedium(misRay, misSI, misTr, sampler1D);
                    Spectrum misLumi(0.0);
                    if (foundIntersection) {
                        if (misSI.getAreaLight() != nullptr && misSI.getAreaLight() == light.get()) {
                            misLumi = misSI.getAreaLight()->lightRadiance(misSI, -misRay.getDirection());
                        }
                    }
                    if (!misLumi.isBlack()) {
                        ret += f * weight * misTr * misLumi / scatteringPdf;
                    }
                }
            }
            // 没有采样到光源
            return ret;
        }

        std::function<void(const int, const int, Sampler *)> PathTracer::render() {
            auto renderFunc = [this](const int row, const int col, Sampler *sampler) -> void {
                // set current sampling pixel
                sampler->forPixel(Point2d(row, col));

                Spectrum ans = {0};
                const double sampleWeight = 1.0 / _samplePerPixel;
                // 做 _samplePerPixel 次采样
                for (int sampleCount = 0; sampleCount < _samplePerPixel; sampleCount++) {
                    MemoryArena arena;
                    auto u = (col + sampler->sample1D()) / double(_camera->getResolutionWidth());
                    auto v = (row + sampler->sample1D()) / double(_camera->getResolutionHeight());

                    Ray sampleRay = _camera->sendRay(u, v);
                    Spectrum shaderColor = shaderOfProgression(sampleRay, *(_scene.get()), sampler, arena);

                    ans += shaderColor;
                    arena.clean();
                    sampler->nextSampleRound();
                }
                ans *= sampleWeight;
                _filmPlane->addSpectrum(ans, row, col);
            };

            return renderFunc;
        }

        Spectrum PathTracer::background(const Ray &ray) {
            // TODO 临时设计背景色
            return Spectrum(0.0f);

            /*
            if (ray.getDirection().z < 0) {
                return Vector3d(0.0f, 0.0f, 0.0f);
            } else {
                return Vector3d(1.0f, 1.0f, 1.0f);
            }

            Vector3d dir = NORMALIZE(ray.getDirection());
            double t = 0.5 * (dir.y + 1.0);
            return (1.0 - t) * Vector3d(1.0, 1.0, 1.0) + t * Vector3d(0.5, 0.7, 1.0);
             */
        }

    }
}