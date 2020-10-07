//
// Created by Storm Phoenix on 2020/9/29.
//
#ifndef KAGUYA_TRIANGLE_H
#define KAGUYA_TRIANGLE_H

#include <kaguya/material/Material.h>
#include <kaguya/math/Math.hpp>
#include <kaguya/scene/Hittable.h>
#include <kaguya/scene/accumulation/AABB.h>

#include <memory>

namespace kaguya {
    namespace scene {

        using kaguya::material::Material;
        using kaguya::scene::acc::AABB;

        // 三角形数据
        class Triangle : public Hittable {
        public:
            Triangle(const Vector3 &a, const Vector3 &b, const Vector3 &c,
                     const Vector3 &normal1, const Vector3 &normal2, const Vector3 &normal3,
                     const Vector2 &uv1, const Vector2 &uv2, const Vector2 &uv3,
                     std::shared_ptr<Material> material, std::shared_ptr<Matrix4> transformMatrix = nullptr);

            bool hit(const Ray &ray, HitRecord &hitRecord, double stepMin, double stepMax);

            const AABB &boundingBox() const;

        private:
            // 定点数据
            const Vector3 _a;
            const Vector3 _b;
            const Vector3 _c;
            const Vector3 _normalA;
            const Vector3 _normalB;
            const Vector3 _normalC;
            const Vector2 _uvA;
            const Vector2 _uvB;
            const Vector2 _uvC;
            std::shared_ptr<Material> _material = nullptr;
            std::shared_ptr<Matrix4> _transformMatrix = nullptr;
            AABB _aabb;
        };

    }
}

#endif //KAGUYA_TRIANGLE_H
