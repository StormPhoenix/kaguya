//
// Created by Storm Phoenix on 2020/11/2.
//

#include <kaguya/Config.h>
#include <kaguya/parallel/RenderPool.h>
#include <kaguya/tracer/Tracer.h>

#include <iostream>

namespace kaguya {
    namespace tracer {

        using kaguya::Config;

        Tracer::Tracer() {}

        void Tracer::run() {
            _scene = Config::nextScene();

            parallel::RenderPool *pool = parallel::RenderPool::getInstance();
            while (_scene != nullptr) {
                _camera = _scene->getCamera();

                assert(_camera != nullptr);
                _filmPlane = _camera->buildFilmPlane(SPECTRUM_CHANNEL);

                // rendering
                auto renderFunc = render();

                // execute rendering task
                int cameraWidth = _camera->getResolutionWidth();
                int cameraHeight = _camera->getResolutionHeight();
                pool->addRenderTask(renderFunc, cameraWidth, cameraHeight);

                // write to image
                std::cout << std::endl << "scene " << _scene->getName() << " completed." << std::endl;
                _filmPlane->writeImage((Config::filenamePrefix + "_" +
                                        _scene->getName() + "_" +
                                        Config::filenameSufix).c_str());

                delete _filmPlane;
                _filmPlane = nullptr;

                _camera = nullptr;
                _scene = Config::nextScene();
            }

            pool->shutdown();
//            delete pool;
        }

        std::shared_ptr<Light> Tracer::uniformSampleLight(std::shared_ptr<Scene> scene, Float *lightPdf, Sampler *sampler) {
            // Sample from multiple light
            auto lights = scene->getLights();
            int nLights = lights.size();
            if (nLights == 0) {
                return nullptr;
            }
            *lightPdf = Float(1.0) / nLights;

            // Randomly select a light
            int lightIndex = std::min(int(sampler->sample1D() * nLights), nLights - 1);
            return lights[lightIndex];
        }

        Spectrum Tracer::sampleDirectLight(std::shared_ptr<Scene> scene, const Interaction &eye,
                                               Sampler *sampler) {
            Float lightPdf = 0;
            auto light = uniformSampleLight(scene, &lightPdf, sampler);
            if (light == nullptr) {
                return 0;
            } else {
                // Sample light spectrum
                return evaluateDirectLight(scene, eye, light, sampler) / lightPdf;
            }
        }

        Spectrum Tracer::evaluateDirectLight(std::shared_ptr<Scene> scene, const Interaction &eye,
                                                 const std::shared_ptr<Light> light, Sampler *sampler) {
            // p(wi)
            Float lightPdf = 0;
            // light dir
            Vector3F wi = Vector3F(0.0);
            // visibility tester
            VisibilityTester visibilityTester;

            Spectrum ret(0);
            // 对光源采样采样
            Spectrum lumi = light->sampleLi(eye, &wi,
                                            &lightPdf, sampler,
                                            &visibilityTester);

            // 排除对光源采样贡献为 0 的情况
            if (lightPdf > math::EPSILON && !lumi.isBlack()) {
                Spectrum f;
                Float scatteringPdf = 0.0f;
                if (eye.isMediumInteraction()) {
                    // handle medium interaction
                    const MediumInteraction &mi = (const MediumInteraction &) eye;
                    assert(mi.getPhaseFunction() != nullptr);

                    scatteringPdf = mi.getPhaseFunction()->scatterPdf(-mi.direction, wi);
                    f = Spectrum(scatteringPdf);
                } else {
                    // handle surface interaction
                    const SurfaceInteraction &si = (const SurfaceInteraction &) eye;
                    assert(si.bsdf != nullptr);

                    Float cosine = ABS_DOT(eye.rendering.normal, wi);
                    scatteringPdf = si.bsdf->samplePdf(-eye.direction, wi);
                    f = si.bsdf->f(-eye.direction, wi) * cosine;
                }

                if (!f.isBlack()) {
                    lumi *= visibilityTester.transmittance(scene, sampler);
                    if (!lumi.isBlack()) {
                        if (light->isDeltaType()) {
                            // shader spectrum
                            ret += f / lightPdf * lumi;
                        } else {
                            // multiple rayImportance sampling
                            Float weight = math::misWeight(1, lightPdf, 1, scatteringPdf);
                            ret += f * weight / lightPdf * lumi;
                        }
                    }
                }
            }

            // multiple importance sampling
            if (!light->isDeltaType()) {
                const Vector3F wo = -eye.direction;
                Spectrum f(0);
                Float scatteringPdf = 0;
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
                    assert(si.bsdf != nullptr);

                    BXDFType sampleType;
                    f = si.bsdf->sampleF(wo, &wi, &scatteringPdf, sampler, BSDF_ALL, &sampleType);
                    f *= ABS_DOT(si.rendering.normal, wi);
                    sampleSpecular = (sampleType & BXDFType::BSDF_SPECULAR) != 0;
                }

                if (!f.isBlack() && scatteringPdf > 0) {
                    Float weight = 1.0;
                    if (!sampleSpecular) {
                        // sample1d light pdf
                        lightPdf = light->pdfLi(eye, wi);
                        if (lightPdf == 0) {
                            return ret;
                        }
                        weight = math::misWeight(1, scatteringPdf, 1, lightPdf);
                    }

                    // find intersection
                    SurfaceInteraction misSI;
                    Spectrum misTr = 1.0;
                    Ray misRay(eye.point, NORMALIZE(wi));
                    bool foundIntersection = scene->intersectWithMedium(misRay, misSI, misTr, sampler);
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
            return ret;
        }

    }
}