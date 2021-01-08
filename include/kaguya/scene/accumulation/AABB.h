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

                AxisAlignBoundingBox(const Vector3d min, const Vector3d max);

                bool insect(const Ray &ray, double minStep, double maxStep) const;

                bool insectPoint(const Ray &ray, double *minStep, double *maxStep) const;

                const Vector3d &min() const {
                    return _min;
                }

                const Vector3d &max() const {
                    return _max;
                }

            private:
                Vector3d _min;
                Vector3d _max;
            };

            typedef AxisAlignBoundingBox AABB;
        }
    }
}

#endif //KAGUYA_AABB_H
