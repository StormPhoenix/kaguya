//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_SPHERE_H
#define KAGUYA_SPHERE_H

#include <kaguya/core/Transform.h>
#include <kaguya/scene/accumulation/AABB.h>
#include <kaguya/math/Math.hpp>
#include <kaguya/scene/meta/Shape.h>
#include <memory>

namespace kaguya {
    namespace scene {
        namespace meta {
            using core::transform::Transform;
            using kaguya::scene::acc::AABB;

            class Sphere : public Shape {
            public:

                /**
                 * 球体
                 * @param center 中心
                 * @param radius 半径
                 * @param material 材质
                 * @param outward 是否向外
                 * @param transformMatrix 变换矩阵
                 */
                Sphere(const Vector3 &center, double radius, bool outward = true,
                       std::shared_ptr<Transform> transformMatrix = std::make_shared<Transform>());

                bool intersect(Ray &ray, SurfaceInteraction &si, double minStep, double maxStep) const override;

                virtual double area() const override {
                    return 4 * math::PI * _radius * _radius;
                }

                virtual SurfaceInteraction sampleSurfacePoint(const Sampler *sampler1D) const override;

                virtual SurfaceInteraction
                sampleSurfaceInteraction(const Interaction &eye, const Sampler *sampler1D) const override;

                virtual double surfaceInteractionPdf(const Interaction &eye, const Vector3 &dir) const override;


                virtual const AABB &bound() const override;

                virtual ~Sphere() {}

            private:
                /**
                 * 计算击中点的法向量
                 * @param hitPoint
                 * @return
                 */
                virtual Vector3 computeNormal(const Vector3 &hitPoint) const;

            private:
                AABB _aabb;
                Vector3 _center;
                Vector3 _transformedCenter;
                double _radius;
                bool _outward;
                std::shared_ptr<Transform> _transformMatrix = nullptr;
                std::shared_ptr<Transform> _inverseTransformMatrix = nullptr;
            };
        }
    }
}

#endif //KAGUYA_SPHERE_H
