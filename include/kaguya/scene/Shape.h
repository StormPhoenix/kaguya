//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_SHAPE_H
#define KAGUYA_SHAPE_H

#include <kaguya/core/Interaction.h>
#include <kaguya/math/Math.hpp>
#include <kaguya/math/Sampler.hpp>
#include <kaguya/scene/accumulation/AABB.h>

namespace kaguya {

    namespace tracer {
        class Ray;
    }

    namespace scene {

        using kaguya::math::random::Sampler1D;
        using kaguya::scene::acc::AABB;
        using kaguya::tracer::Ray;
        using kaguya::core::SurfaceInteraction;
        using kaguya::core::Interaction;

        class Shape {
        public:
            /**
             * 击中判定
             * @param ray
             * @param si
             * @param stepMin 射线步长最小值
             * @param stepMax 射线步长最大值
             * @return
             */
            virtual bool insect(Ray &ray, SurfaceInteraction &si, double stepMin, double stepMax) const = 0;

            /**
             * Shape area
             * @return
             */
            virtual double area() const = 0;

            /**
             * Sample point on shape surface
             * @return
             */
            virtual SurfaceInteraction sampleSurfacePoint(const Sampler1D *sampler1D) const = 0;

            /**
             * Get pdf of point on shape surface
             * @param si
             * @return
             */
            virtual double surfacePointPdf(const SurfaceInteraction &si) const;

            /**
             * TODO add parameter pdf
             * Sample a ray origins from @param eye, let it insect the shape and get the interaction.
             * @return
             */
            virtual SurfaceInteraction sampleSurfaceInteraction(
                    const Interaction &eye,
                    const Sampler1D *sampler1D) const;

            /**
             * Get pdf of the ray which origins from @param eye
             * @return
             */
            virtual double surfaceInteractionPdf(const Interaction &eye, const Vector3 &dir) const;

            virtual const AABB &boundingBox() const = 0;

            virtual const long long getId() const;

            virtual void setId(long long id);

        protected:
            long long _id = -1;
        };

    }
}

#endif //KAGUYA_SHAPE_H
