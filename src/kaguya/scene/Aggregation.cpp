//
// Created by Storm Phoenix on 2020/12/23.
//

#include <kaguya/scene/Aggregation.h>
#include <kaguya/scene/accumulation/AABB.h>

namespace kaguya {
    namespace scene {

        using kaguya::scene::acc::AABB;

        bool Aggregation::intersect(Ray &ray, SurfaceInteraction &si, Float minStep, Float maxStep) const {
            const std::vector<std::shared_ptr<Intersectable>> objects = aggregation();
            if (objects.size() == 0) {
                return false;
            }

            bool foundIntersection = false;
            for (auto it = objects.begin(); it != objects.end(); it++) {
                foundIntersection =
                        it->get()->intersect(ray, si, minStep, foundIntersection ? ray.getStep() : maxStep) ||
                        foundIntersection;
            }
            return foundIntersection;
        }

        AABB Aggregation::mergeAABB(const AABB &a, const AABB &b) const {
            Vector3F small(std::min(a.min().x, b.min().x),
                           std::min(a.min().y, b.min().y),
                           std::min(a.min().z, b.min().z));
            Vector3F big(std::max(a.max().x, b.max().x),
                         std::max(a.max().y, b.max().y),
                         std::max(a.max().z, b.max().z));
            return AABB(small, big);
        }

        AABB Aggregation::mergeBoundingBox(const std::vector<std::shared_ptr<Intersectable>> &objects) const {
            AABB bound(Vector3F(0), Vector3F(0));
            if (objects.size() == 0) {
                return bound;
            }

            for (auto it = objects.begin(); it != objects.end(); it++) {
                bound = mergeAABB(bound, it->get()->boundingBox());
            }
            return bound;
        }
    }
}