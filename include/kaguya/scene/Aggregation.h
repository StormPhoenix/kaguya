//
// Created by Storm Phoenix on 2020/12/21.
//

#ifndef KAGUYA_AGGREGATION_H
#define KAGUYA_AGGREGATION_H

#include <kaguya/scene/Geometry.h>

#include <vector>

namespace kaguya {
    namespace scene {

        class Aggregation : public Intersectable {
        public:
            virtual bool intersect(Ray &ray, SurfaceInteraction &si, double minStep, double maxStep) const override;

            virtual const std::vector<std::shared_ptr<Intersectable>> aggregation() const = 0;

        protected:

            AABB mergeAABB(const AABB &a, const AABB &b) const;

            AABB mergeBoundingBox(const std::vector<std::shared_ptr<Intersectable>> &objects) const;
        };

    }
}

#endif //KAGUYA_AGGREGATION_H
