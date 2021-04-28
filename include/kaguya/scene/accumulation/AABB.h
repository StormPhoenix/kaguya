//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_AABB_H
#define KAGUYA_AABB_H

#include <kaguya/math/Math.h>
#include <kaguya/tracer/Ray.h>

namespace kaguya {
    namespace scene {
        namespace acc {

            using kaguya::tracer::Ray;

            /**
             * 物体沿坐标轴线包围盒
             */
            class AxisAlignBoundingBox {
            public:
                AxisAlignBoundingBox() {}

                AxisAlignBoundingBox(const Vector3F min, const Vector3F max);

                bool insect(const Ray &ray, Float minStep, Float maxStep) const;

                bool insectPoint(const Ray &ray, Float *minStep, Float *maxStep) const;

                const Vector3F &minPos() const {
                    return _min;
                }

                const Vector3F &maxPos() const {
                    return _max;
                }

            private:
                Vector3F _min;
                Vector3F _max;
            };

            typedef AxisAlignBoundingBox AABB;
        }
    }
}

#endif //KAGUYA_AABB_H
