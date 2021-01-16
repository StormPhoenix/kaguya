//
// Created by Storm Phoenix on 2020/10/7.
//

#include <kaguya/scene/accumulation/BVH.h>
#include <kaguya/sampler/Sampler.h>
#include <kaguya/sampler/DefaultSampler.h>

namespace kaguya {
    namespace scene {
        namespace acc {

            using namespace sampler;

            inline bool compareBox(const std::shared_ptr<Intersectable> a,
                                   const std::shared_ptr<Intersectable> b, int axis) {
                AABB boxA = a->boundingBox();
                AABB boxB = b->boundingBox();

                return boxA.min()[axis] < boxB.min()[axis];
            }

            bool compareX(const std::shared_ptr<Intersectable> a, const std::shared_ptr<Intersectable> b) {
                return compareBox(a, b, 0);
            }

            bool compareY(const std::shared_ptr<Intersectable> a, const std::shared_ptr<Intersectable> b) {
                return compareBox(a, b, 1);
            }

            bool compareZ(const std::shared_ptr<Intersectable> a, const std::shared_ptr<Intersectable> b) {
                return compareBox(a, b, 2);
            }

            BVH::BVH(std::shared_ptr<Intersectable> object) {
                std::vector<std::shared_ptr<Intersectable>> objects;
                objects.push_back(object);
                build(objects, 0, 1);
            }

            BVH::BVH(std::vector<std::shared_ptr<Intersectable>> &objects) : BVH(objects, 0, objects.size()) {}

            BVH::BVH(std::vector<std::shared_ptr<Intersectable>> &objects, size_t start, size_t end) {
                build(objects, start, end);
            }

            bool BVH::intersect(Ray &ray, SurfaceInteraction &si,
                                Float minStep, Float maxStep) const {
                // 在 insect 之前，BVH 必须已经构建完毕
                assert(_isValid);

                if (_aabb.insect(ray, minStep, maxStep)) {
                    bool leftHit = _left->intersect(ray, si, minStep, maxStep);
                    bool rightHit = _right->intersect(ray, si, minStep, leftHit ? ray.getStep() : maxStep);
                    return leftHit || rightHit;
                } else {
                    return false;
                }
            }

            void BVH::build(std::vector<std::shared_ptr<Intersectable>> &objects,
                            size_t start, size_t end) {
                Sampler *sampler = DefaultSampler::newInstance();
                // 采用最简单的平均分配法
                int axis = math::randomInt(0, 2, sampler);
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
                _aabb = mergeAABB(leftAABB, rightAABB);
                _isValid = true;

                _objects.push_back(_left);
                _objects.push_back(_right);

                delete sampler;
            }

            const AABB &BVH::boundingBox() const {
                return _aabb;
            }

            const std::vector<std::shared_ptr<Intersectable>> BVH::aggregation() const {
                return _objects;
            }
        }
    }
}