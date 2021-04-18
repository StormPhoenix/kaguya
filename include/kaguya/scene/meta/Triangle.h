//
// Created by Storm Phoenix on 2020/9/29.
//
#ifndef KAGUYA_TRIANGLE_H
#define KAGUYA_TRIANGLE_H

#include <kaguya/core/Transform.h>
#include <kaguya/material/Material.h>
#include <kaguya/math/Math.h>
#include <kaguya/scene/meta/Shape.h>
#include <kaguya/scene/accumulation/AABB.h>

#include <memory>

namespace kaguya {
    namespace scene {
        namespace meta {
            using core::transform::Transform;
            using kaguya::material::Material;
            using kaguya::scene::acc::AABB;

            // 三角形数据
            class Triangle : public Shape {
            public:
                typedef std::shared_ptr<Triangle> Ptr;

                Triangle(const Vector3F &a, const Vector3F &b, const Vector3F &c,
                         const Normal3F &normal1, const Normal3F &normal2, const Normal3F &normal3,
                         const Vector2F &uv1, const Vector2F &uv2, const Vector2F &uv3,
                         std::shared_ptr<Transform> transformMatrix = std::make_shared<Transform>());

                bool intersect(Ray &ray, SurfaceInteraction &si, Float minStep, Float maxStep) const override;

                Float area() const override;

                SurfaceInteraction sampleSurfacePoint(Sampler *sampler) const override;

                const AABB &bound() const override;

                virtual ~Triangle() {}

            private:
                /**
                 * 初始化内部数据
                 */
                void init();

            private:
                // 定点数据
                const Vector3F _p1;
                const Vector3F _p2;
                const Vector3F _p3;

                // world space 中的顶点坐标
                Point3F _p1World;
                Point3F _p2World;
                Point3F _p3World;

                // 采用 geometry normal
                bool usingNg = false;

                const Normal3F _n1;
                const Normal3F _n2;
                const Normal3F _n3;

                // world space 中的法线坐标
                Normal3F _n1World;
                Normal3F _n2World;
                Normal3F _n3World;

                const Vector2f _uv1;
                const Vector2f _uv2;
                const Vector2f _uv3;
                std::shared_ptr<Transform> _transformMatrix = nullptr;
                AABB _aabb;
            };
        }
    }
}

#endif //KAGUYA_TRIANGLE_H
