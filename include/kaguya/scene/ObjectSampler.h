//
// Created by Storm Phoenix on 2020/10/9.
//

#ifndef KAGUYA_OBJECTSAMPLER_H
#define KAGUYA_OBJECTSAMPLER_H

#include <kaguya/math/Math.hpp>
#include <kaguya/tracer/Ray.h>
#include <kaguya/scene/Hittable.h>

namespace kaguya {
    namespace scene {

        using kaguya::tracer::Ray;

        /**
         * 实现对场景中物体的采样功能
         */
        class ObjectSampler : public Hittable {
        public:
            /**
             * 在物体上采样某一点
             * @param pdf 采样概率
             * @param normal 采样点处法线
             * @return
             */
            virtual Vector3 samplePoint(double &pdf, Vector3 &normal) = 0;

            /**
             * 计算某一点在物体上的 pdf
             * @param point
             * @return
             */
            virtual double samplePointPdf(Vector3 &point) = 0;

        };
    }
}

#endif //KAGUYA_OBJECTSAMPLER_H
