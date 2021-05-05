//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_SPHERE_H
#define KAGUYA_SPHERE_H

#include <kaguya/core/Transform.h>
#include <kaguya/scene/accumulation/AABB.h>
#include <kaguya/math/Math.h>
#include <kaguya/scene/meta/Shape.h>
#include <memory>

namespace kaguya {
    namespace scene {
        namespace meta {
            using core::transform::Transform;
            using kaguya::scene::acc::AABB;

            class Sphere : public Shape {
            public:
                Sphere(const Vector3F &center, Float radius, bool outward = true);

                Sphere(std::shared_ptr<Transform> transformMatrix = std::make_shared<Transform>(), bool outward = true);

                bool intersect(Ray &ray, SurfaceInteraction &si, Float minStep, Float maxStep) const override;

                virtual Float area() const override {
                    return 4 * math::PI * _radius * _radius;
                }

                virtual SurfaceInteraction sampleSurfacePoint(Float *pdf, Sampler *sampler) const override;

                virtual SurfaceInteraction
                sampleSurfaceInteraction(const Interaction &eye, Float *pdf, Sampler *sampler) const override;

                virtual Float surfaceInteractionPdf(const Interaction &eye, const Vector3F &dir) const override;


                virtual const AABB &bound() const override;

                virtual ~Sphere() {}

            private:
                /**
                 * 计算击中点的法向量
                 * @param hitPoint
                 * @return
                 */
                virtual Vector3F computeNormal(const Vector3F &hitPoint) const;

            private:
                AABB _aabb;
                Vector3F _center;
                Vector3F _transformedCenter;
                Float _radius;
                bool _outward;
                std::shared_ptr<Transform> _transformMatrix = nullptr;
                std::shared_ptr<Transform> _inverseTransformMatrix = nullptr;
            };
        }
    }
}

#endif //KAGUYA_SPHERE_H
