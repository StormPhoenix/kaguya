//
// Created by Storm Phoenix on 2020/10/7.
//

#include <kaguya/scene/accumulation/AABB.h>

namespace kaguya {
    namespace scene {
        namespace acc {

            AxisAlignBoundingBox::AxisAlignBoundingBox(const Vector3F min, const Vector3F max) : _min(min), _max(max) {}

            bool AxisAlignBoundingBox::insectPoint(const Ray &ray, Float *minStep, Float *maxStep) const {
                Float t0 = 0, t1 = ray.getStep();
                for (int axis = 0; axis < 3; ++axis) {
                    Float invStep = 1 / ray.getDirection()[axis];
                    Float near = (_min[axis] - ray.getOrigin()[axis]) * invStep;
                    Float far = (_max[axis] - ray.getOrigin()[axis]) * invStep;

                    if (near > far) {
                        std::swap(near, far);
                    }

                    t0 = near > t0 ? near : t0;
                    t1 = far < t1 ? far : t1;
                    if (t0 > t1) {
                        return false;
                    }
                }

                if (minStep != nullptr) {
                    *minStep = t0;
                }
                if (maxStep != nullptr) {
                    *maxStep = t1;
                }
                return true;
            }

            bool AxisAlignBoundingBox::insect(const Ray &ray, Float minStep, Float maxStep) const {
                for (int axis = 0; axis < 3; axis++) {
                    Float invStep = 1.0 / ray.getDirection()[axis];
                    Float t0 = (_min[axis] - ray.getOrigin()[axis]) * invStep;
                    Float t1 = (_max[axis] - ray.getOrigin()[axis]) * invStep;

                    if (invStep < 0.0f) {
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