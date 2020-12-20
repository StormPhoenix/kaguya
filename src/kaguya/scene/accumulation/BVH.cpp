//
// Created by Storm Phoenix on 2020/10/7.
//

#include <kaguya/scene/accumulation/BVH.h>
#include <kaguya/math/Sampler.hpp>

namespace kaguya {
    namespace scene {
        namespace acc {

            inline bool compareBox(const std::shared_ptr<Shape> a, const std::shared_ptr<Shape> b, int axis) {
                AABB boxA = a->boundingBox();
                AABB boxB = b->boundingBox();

                return boxA.min()[axis] < boxB.min()[axis];
            }

            bool compareX(const std::shared_ptr<Shape> a, const std::shared_ptr<Shape> b) {
                return compareBox(a, b, 0);
            }

            bool compareY(const std::shared_ptr<Shape> a, const std::shared_ptr<Shape> b) {
                return compareBox(a, b, 1);
            }

            bool compareZ(const std::shared_ptr<Shape> a, const std::shared_ptr<Shape> b) {
                return compareBox(a, b, 2);
            }

            BVH::BVH(std::shared_ptr<Shape> object) {
                std::vector<std::shared_ptr<Shape>> objects;
                objects.push_back(object);
                build(objects, 0, 1);
            }

            BVH::BVH(std::vector<std::shared_ptr<Shape>> &objects) : BVH(objects, 0, objects.size()) {}

            BVH::BVH(std::vector<std::shared_ptr<Shape>> &objects, size_t start, size_t end) {
                build(objects, start, end);
            }

            bool BVH::insect(Ray &ray, SurfaceInteraction &si,
                             double stepMin, double stepMax) const {
                // 在 insect 之前，BVH 必须已经构建完毕
                assert(_isValid);

                if (_aabb.insect(ray, stepMin, stepMax)) {
                    bool leftHit = _left->insect(ray, si, stepMin, stepMax);
                    bool rightHit = _right->insect(ray, si, stepMin, leftHit ? ray.getStep() : stepMax);
                    return leftHit || rightHit;
                } else {
                    return false;
                }
            }

            void BVH::build(std::vector<std::shared_ptr<Shape>> &objects,
                            size_t start, size_t end) {
                math::random::Sampler1D *sampler = math::random::Sampler1D::newInstance();
                // 采用最简单的平均分配法
                int axis = randomInt(0, 2, sampler);
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

                delete sampler;
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