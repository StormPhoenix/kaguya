//
// Created by Storm Phoenix on 2020/12/23.
//

#ifndef KAGUYA_INTERSECTABLE_H
#define KAGUYA_INTERSECTABLE_H

#include <kaguya/core/Interaction.h>
#include <kaguya/scene/accumulation/AABB.h>
#include <kaguya/tracer/Ray.h>

namespace kaguya {
    namespace scene {

        using kaguya::core::SurfaceInteraction;
        using kaguya::scene::acc::AABB;
        using kaguya::tracer::Ray;

        class Intersectable {
        public:
            /**
             * 求交判定
             * @param ray
             * @param si
             * @param minStep 射线步长最小值
             * @param maxStep 射线步长最大值
             * @return
             */
            virtual bool intersect(Ray &ray, SurfaceInteraction &si, double minStep, double maxStep) const = 0;

            /**
             * Bounding box
             * @return
             */
            virtual const AABB &boundingBox() const = 0;
        };

    }
}

#endif //KAGUYA_INTERSECTABLE_H
