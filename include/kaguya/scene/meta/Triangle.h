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

                Triangle(const Vector3d &a, const Vector3d &b, const Vector3d &c,
                         const Vector3d &normal1, const Vector3d &normal2, const Vector3d &normal3,
                         const Vector2d &uv1, const Vector2d &uv2, const Vector2d &uv3,
                         std::shared_ptr<Transform> transformMatrix = std::make_shared<Transform>());

                bool intersect(Ray &ray, SurfaceInteraction &si, double minStep, double maxStep) const override;

                double area() const override;

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
                const Vector3d _position1;
                const Vector3d _position2;
                const Vector3d _position3;

                // world space 中的顶点坐标
                Vector3d _transformedPosition1;
                Vector3d _transformedPosition2;
                Vector3d _transformedPosition3;

                const Vector3d _normal1;
                const Vector3d _normal2;
                const Vector3d _normal3;

                // world space 中的法线坐标
                Vector3d _transformedNormal1;
                Vector3d _transformedNormal2;
                Vector3d _transformedNormal3;

                const Vector2d _uv1;
                const Vector2d _uv2;
                const Vector2d _uv3;
                std::shared_ptr<Transform> _transformMatrix = nullptr;
                AABB _aabb;
            };
        }
    }
}

#endif //KAGUYA_TRIANGLE_H
