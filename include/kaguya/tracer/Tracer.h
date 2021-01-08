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

namespace kaguya {
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

        private:
            // Rendering implementation
            virtual std::function<void(const int, const int, Sampler *)> render() = 0;

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
