//
// Created by Storm Phoenix on 2020/9/29.
//
#ifndef KAGUYA_TRIANGLE_H
#define KAGUYA_TRIANGLE_H

#include <kaguya/material/Material.h>
#include <kaguya/math/Math.hpp>
#include <kaguya/scene/Hittable.h>
#include <kaguya/scene/meta/Vertex.h>
#include <kaguya/scene/accumulation/AABB.h>

#include <memory>

namespace kaguya {
    namespace scene {

        using kaguya::material::Material;
        using kaguya::scene::acc::AABB;

        // 三角形数据
        class Triangle : public Hittable {
        public:
            Triangle(const Vertex &a, const Vertex &b, const Vertex &c,
                     std::shared_ptr<Material> material, std::shared_ptr<Matrix4> transformMatrix = nullptr);

            Triangle(const Vector3 &a, const Vector3 &b, const Vector3 &c,
                     const Vector3 &normal1, const Vector3 &normal2, const Vector3 &normal3,
                     const Vector2 &uv1, const Vector2 &uv2, const Vector2 &uv3,
                     std::shared_ptr<Material> material, std::shared_ptr<Matrix4> transformMatrix = nullptr);

            bool hit(const Ray &ray, HitRecord &hitRecord, double stepMin, double stepMax);

            const AABB &boundingBox() const;

        private:
            /**
             * 初始化内部数据
             */
            void init();

        private:
            // 定点数据
            const Vector3 _position1;
            const Vector3 _position2;
            const Vector3 _position3;
            const Vector3 _normal1;
            const Vector3 _normal2;
            const Vector3 _normal3;
            const Vector2 _uv1;
            const Vector2 _uv2;
            const Vector2 _uv3;
            std::shared_ptr<Material> _material = nullptr;
            std::shared_ptr<Matrix4> _transformMatrix = nullptr;
            AABB _aabb;
        };

    }
}

#endif //KAGUYA_TRIANGLE_H
