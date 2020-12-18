//
// Created by Storm Phoenix on 2020/10/7.
//

#include <kaguya/scene/accumulation/AABB.h>

namespace kaguya {
    namespace scene {
        namespace acc {

            AxisAlignBoundingBox::AxisAlignBoundingBox(const Vector3 min, const Vector3 max) : _min(min), _max(max) {}

            bool AxisAlignBoundingBox::hit(const Ray &ray, double minStep, double maxStep) const {
                for (int axis = 0; axis < 3; axis++) {
                    double subStep = 1.0 / ray.getDirection()[axis];
                    double t0 = (_min[axis] - ray.getOrigin()[axis]) * subStep;
                    double t1 = (_max[axis] - ray.getOrigin()[axis]) * subStep;

                    if (subStep < 0.0f) {
                        std::swap(t0, t1);
                    }
                    minStep = t0 > minStep ? t0 : minStep;
                    maxStep = t1 < maxStep ? t1 : maxStep;
                    if (maxStep <= minStep) {
                        return false;
                    }
                }
                return true;
            }
        }
    }
}