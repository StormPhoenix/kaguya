//
// Created by Storm Phoenix on 2020/10/7.
//

#include <kaguya/scene/accumulation/BVH.h>

namespace kaguya {
    namespace scene {
        namespace acc {

            inline bool compareBox(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis) {
                AABB boxA = a->boundingBox();
                AABB boxB = b->boundingBox();

                return boxA.min()[axis] < boxB.min()[axis];
            }

            bool compareX(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
                return compareBox(a, b, 0);
            }

            bool compareY(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
                return compareBox(a, b, 1);
            }

            bool compareZ(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
                return compareBox(a, b, 2);
            }

            BVH::BVH(std::shared_ptr<Hittable> object) {
                std::vector<std::shared_ptr<Hittable>> objects;
                objects.push_back(object);
                build(objects, 0, 1);
            }

            BVH::BVH(std::vector<std::shared_ptr<Hittable>> &objects) : BVH(objects, 0, objects.size()) {}

            BVH::BVH(std::vector<std::shared_ptr<Hittable>> &objects, size_t start, size_t end) {
                build(objects, start, end);
            }

            bool BVH::hit(const Ray &ray, HitRecord &hitRecord,
                          double stepMin, double stepMax) {
                // 在 hit 之前，BVH 必须已经构建完毕
                assert(_isValid);

                if (_aabb.hit(ray, stepMin, stepMax)) {
                    bool leftHit = _left->hit(ray, hitRecord, stepMin, stepMax);
                    bool rightHit = _right->hit(ray, hitRecord, stepMin, leftHit ? hitRecord.step : stepMax);
                    return leftHit || rightHit;
                } else {
                    return false;
                }
            }

            void BVH::build(std::vector<std::shared_ptr<Hittable>> &objects,
                            size_t start, size_t end) {
                // 采用最简单的平均分配法
                int axis = randomInt(0, 2);
                auto comparator = (axis == 0) ? compareX
                                              : (axis == 1) ? compareY
                                                            : compareZ;

                size_t range = end - start;
                if (range == 1) {
                    _left = _right = objects[start];
                } else if (range == 2) {
                    if (comparator(objects[start], objects[start + 1])) {
                        _left = objects[start];
                        _right = objects[start + 1];
                    } else {
                        _left = objects[start + 1];
                        _right = objects[start];
                    }
                } else {
                    std::sort(objects.begin() + start, objects.begin() + end, comparator);
                    auto mid = start + range / 2;
                    _left = std::make_shared<BVH>(objects, start, mid);
                    _right = std::make_shared<BVH>(objects, mid, end);
                }

                const AABB &leftAABB = _left->boundingBox();
                const AABB &rightAABB = _right->boundingBox();
                mergeBoundingBox(leftAABB, rightAABB);
                _isValid = true;
            }

            void BVH::mergeBoundingBox(const AABB &leftBox, const AABB &rightBox) {
                Vector3 small(std::min(leftBox.min().x, rightBox.min().x),
                              std::min(leftBox.min().y, rightBox.min().y),
                              std::min(leftBox.min().z, rightBox.min().z));
                Vector3 big(std::max(leftBox.max().x, rightBox.max().x),
                            std::max(leftBox.max().y, rightBox.max().y),
                            std::max(leftBox.max().z, rightBox.max().z));
                _aabb = AABB(small, big);
            }

            const AABB &BVH::boundingBox() const {
                return _aabb;
            }
        }
    }
}