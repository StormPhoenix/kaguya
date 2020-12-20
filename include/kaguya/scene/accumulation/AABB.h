//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_AABB_H
#define KAGUYA_AABB_H

#include <kaguya/math/Math.hpp>
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

                AxisAlignBoundingBox(const Vector3 min, const Vector3 max);

                bool insect(const Ray &ray, double minStep, double maxStep) const;

                bool insectPoint(const Ray &ray, double *minStep, double *maxStep) const;

                const Vector3 &min() const {
                    return _min;
                }

                const Vector3 &max() const {
                    return _max;
                }

            private:
                Vector3 _min;
                Vector3 _max;
            };

            typedef AxisAlignBoundingBox AABB;
        }
    }
}

#endif //KAGUYA_AABB_H
