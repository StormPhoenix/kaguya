//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_SHAPE_H
#define KAGUYA_SHAPE_H

#include <kaguya/core/Interaction.h>
#include <kaguya/math/Math.hpp>
#include <kaguya/math/Sampler.hpp>
#include <kaguya/scene/accumulation/AABB.h>
#include <kaguya/scene/Intersectable.h>

namespace kaguya {

    namespace tracer {
        class Ray;
    }

    namespace scene {
        namespace meta {
            using kaguya::math::random::Sampler1D;
            using kaguya::scene::acc::AABB;
            using kaguya::tracer::Ray;
            using kaguya::core::SurfaceInteraction;
            using kaguya::core::Interaction;

            class Shape {
            public:
                virtual bool intersect(Ray &ray, SurfaceInteraction &si, double minStep, double maxStep) const = 0;

                virtual const AABB &bound() const = 0;

                virtual double area() const = 0;

                virtual SurfaceInteraction sampleSurfacePoint(const Sampler1D *sampler1D) const = 0;

                virtual double surfacePointPdf(const SurfaceInteraction &si) const;

                virtual SurfaceInteraction sampleSurfaceInteraction(
                        const Interaction &eye, const Sampler1D *sampler1D) const;

                virtual double surfaceInteractionPdf(const Interaction &eye, const Vector3 &dir) const;
            };
        }
    }
}

#endif //KAGUYA_SHAPE_H
