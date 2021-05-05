//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_SHAPE_H
#define KAGUYA_SHAPE_H

#include <kaguya/core/Interaction.h>
#include <kaguya/math/Math.h>
#include <kaguya/sampler/Sampler.h>
#include <kaguya/scene/accumulation/AABB.h>
#include <kaguya/scene/Intersectable.h>

namespace kaguya {

    namespace tracer {
        class Ray;
    }

    namespace scene {
        namespace meta {
            using kaguya::sampler::Sampler;
            using kaguya::scene::acc::AABB;
            using kaguya::tracer::Ray;
            using kaguya::core::SurfaceInteraction;
            using kaguya::core::Interaction;

            class Shape {
            public:

                typedef std::shared_ptr<Shape> Ptr;

                virtual bool intersect(Ray &ray, SurfaceInteraction &si, Float minStep, Float maxStep) const = 0;

                virtual const AABB &bound() const = 0;

                virtual Float area() const = 0;

                virtual SurfaceInteraction sampleSurfacePoint(Float *pdf, Sampler *sampler) const = 0;

                virtual Float surfacePointPdf(const SurfaceInteraction &si) const;

                virtual SurfaceInteraction sampleSurfaceInteraction(
                        const Interaction &eye, Float *pdf, Sampler *sampler) const;

                virtual Float surfaceInteractionPdf(const Interaction &eye, const Vector3F &dir) const;
            };
        }
    }
}

#endif //KAGUYA_SHAPE_H
