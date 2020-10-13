//
// Created by Storm Phoenix on 2020/10/9.
//

#ifndef KAGUYA_SHAPESAMPLER_H
#define KAGUYA_SHAPESAMPLER_H

#include <kaguya/math/Math.hpp>
#include <kaguya/tracer/Ray.h>
#include <kaguya/scene/Shape.h>

namespace kaguya {
    namespace scene {

        using kaguya::tracer::Ray;

        /**
         * 实现对场景中物体的采样功能
         */
        class ShapeSampler : public Shape {
        public:
            /**
             * Shape 的表面积
             * @return
             */
            virtual double area() = 0;

            /**
             * 在 Shape 表面按照 Uniform 采样点
             * @return
             */
            virtual Interaction sample() = 0;

            /**
             * 计算
             * @param point 采样点
             * @return
             */
            virtual double pdf(Interaction &point);

            /**
             * 在 Shape 表面按照 Uniform 采样点，同时保证对 point 的可见性，并返回概率 pdf
             * @param eye
             * @return
             */
            virtual Interaction sample(const Interaction &eye);

            /**
             * 计算从 eye 出发，沿 dir 方向击中 Shape 上的点的概率
             * @param eye
             * @param dir
             * @param pdf
             * @return
             */
            virtual double pdf(const Interaction &eye, const Vector3 &dir);
        };
    }
}

#endif //KAGUYA_SHAPESAMPLER_H
