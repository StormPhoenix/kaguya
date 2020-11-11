//
// Created by Storm Phoenix on 2020/10/8.
//

#ifndef KAGUYA_TRACER_H
#define KAGUYA_TRACER_H

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

#include <kaguya/tracer/Camera.h>
#include <kaguya/tracer/FilmPlane.h>

namespace kaguya {
    namespace tracer {

        using namespace kaguya::core;

        class Tracer {
        public:
            virtual void run() = 0;

        protected:
            /**
             * 渲染结果写入位图
             * @param color
             */
            void writeShaderColor(const Spectrum &spectrum, int row, int col);

        protected:
            // 相机
            std::shared_ptr<Camera> _camera = nullptr;
            // 渲染结果
            FilmPlane *_filmPlane = nullptr;
        };

    }
}

#endif //KAGUYA_TRACER_H
