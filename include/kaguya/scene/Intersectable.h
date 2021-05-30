//
// Created by Storm Phoenix on 2020/12/23.
//

#ifndef KAGUYA_INTERSECTABLE_H
#define KAGUYA_INTERSECTABLE_H

#include <kaguya/core/Interaction.h>
#include <kaguya/scene/accumulation/AABB.h>
#include <kaguya/tracer/Ray.h>

namespace RENDER_NAMESPACE {
    namespace scene {

        using core::SurfaceInteraction;
        using scene::acc::AABB;
        using tracer::Ray;

        class Intersectable {
        public:
            typedef std::shared_ptr<Intersectable> Ptr;

            /**
             * Interseciton check
             * @param ray
             * @param si
             * @param minStep ray min step
             * @param maxStep ray max step
             * @return
             */
            virtual bool intersect(Ray &ray, SurfaceInteraction &si, Float minStep, Float maxStep) const = 0;

            /**
             * Bounding box
             * @return
             */
            virtual const AABB &boundingBox() const = 0;
        };

    }
}

#endif //KAGUYA_INTERSECTABLE_H
