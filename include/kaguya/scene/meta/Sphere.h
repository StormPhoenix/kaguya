//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_SPHERE_H
#define KAGUYA_SPHERE_H

#include <kaguya/scene/accumulation/AABB.h>
#include <kaguya/math/Math.hpp>
#include <kaguya/scene/Hittable.h>
#include <memory>

namespace kaguya {
    namespace scene {

        using kaguya::scene::acc::AABB;

        class Sphere : public Hittable {
        public:

            Sphere(const Vector3 &center, double radius, std::shared_ptr<Material> material,
                   std::shared_ptr<Matrix4> transformMatrix = nullptr);

            bool hit(const Ray &ray, HitRecord &hitRecord, double stepMin, double stepMax) override;

            const AABB &boundingBox() const override;

        private:
            AABB _aabb;
            Vector3 _center;
            Vector3 _transformedCenter;
            double _radius;
            std::shared_ptr<Material> _material = nullptr;
            std::shared_ptr<Matrix4> _transformMatrix = nullptr;
            std::shared_ptr<Matrix4> _inverseTransformMatrix = nullptr;
        };

    }
}

#endif //KAGUYA_SPHERE_H
