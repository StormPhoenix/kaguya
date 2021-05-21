//
// Created by Storm Phoenix on 2020/10/8.
//

#ifndef KAGUYA_TRACER_H
#define KAGUYA_TRACER_H

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

#include <kaguya/scene/Scene.h>
#include <kaguya/tracer/Camera.h>
#include <kaguya/tracer/FilmPlane.h>

namespace RENDER_NAMESPACE {
    namespace tracer {

        using namespace kaguya::core;
        using kaguya::scene::Scene;

        class Tracer {
        public:
            Tracer();

            void run();

            virtual ~Tracer() {
                if (_filmPlane != nullptr) {
                    delete (_filmPlane);
                }
            }

        protected:
            virtual std::shared_ptr<Light>
            uniformSampleLight(std::shared_ptr<Scene> scene, Float *lightPdf, Sampler *sampler);

            /**
            * 在 eye 处对光源采样
            * @param scene scene
            * @param eye
            * @param sampleRay eye 处出发的射线
            * @param lightPdf 采样概率
            * @return
            */
            Spectrum sampleDirectLight(std::shared_ptr<Scene> scene, const Interaction &eye,
                                       Sampler *sampler);

            virtual Spectrum evaluateDirectLight(std::shared_ptr<Scene> scene, const Interaction &eye,
                                                 const std::shared_ptr<Light> light, Sampler *sampler);

            virtual void writeImage(std::string filename, Float weight = 1.0f);

        private:
            // Rendering implementation
            virtual void render() = 0;

        protected:
            // 场景
            std::shared_ptr<Scene> _scene = nullptr;
            // 相机
            std::shared_ptr<Camera> _camera = nullptr;
            // 成像平面
            FilmPlane *_filmPlane = nullptr;
        };

    }
}

#endif //KAGUYA_TRACER_H
