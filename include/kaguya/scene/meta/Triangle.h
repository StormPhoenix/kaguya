//
// Created by Storm Phoenix on 2020/9/29.
//
#ifndef KAGUYA_TRIANGLE_H
#define KAGUYA_TRIANGLE_H

#include <kaguya/core/Transform.h>
#include <kaguya/material/Material.h>
#include <kaguya/math/Math.h>
#include <kaguya/scene/meta/Shape.h>
#include <kaguya/scene/meta/Vertex.h>
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
                Triangle(const Vertex &a, const Vertex &b, const Vertex &c,
                         std::shared_ptr<Transform> transformMatrix = std::make_shared<Transform>());

                Triangle(const Vector3F &a, const Vector3F &b, const Vector3F &c,
                         const Normal3F &normal1, const Normal3F &normal2, const Normal3F &normal3,
                         const Vector2F &uv1, const Vector2F &uv2, const Vector2F &uv3,
                         std::shared_ptr<Transform> transformMatrix = std::make_shared<Transform>());

                bool intersect(Ray &ray, SurfaceInteraction &si, Float minStep, Float maxStep) const override;

                Float area() const override;

                SurfaceInteraction sampleSurfacePoint(Sampler *sampler1D) const override;

                const AABB &bound() const override;

                virtual ~Triangle() {}

            private:
                /**
                 * 初始化内部数据
                 */
                void init();

            private:
                // 定点数据
                const Vector3F _position1;
                const Vector3F _position2;
                const Vector3F _position3;

                // world space 中的顶点坐标
                Point3F _transformedPosition1;
                Point3F _transformedPosition2;
                Point3F _transformedPosition3;

                const Normal3F _normal1;
                const Normal3F _normal2;
                const Normal3F _normal3;

                // world space 中的法线坐标
                Normal3F _transformedNormal1;
                Normal3F _transformedNormal2;
                Normal3F _transformedNormal3;

                Normal3F _geometryNormal;

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
