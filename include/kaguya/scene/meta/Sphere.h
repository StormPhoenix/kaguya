//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_SPHERE_H
#define KAGUYA_SPHERE_H

#include <kaguya/scene/accumulation/AABB.h>
#include <kaguya/math/Math.hpp>
#include <kaguya/scene/Shape.h>
#include <memory>

namespace kaguya {
    namespace scene {

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
            Sphere(const Vector3 &center, double radius, std::shared_ptr<Material> material, bool outward = true,
                   std::shared_ptr<Matrix4> transformMatrix = nullptr);

            bool hit(const Ray &ray, Interaction &hitRecord, double stepMin, double stepMax) override;

            const AABB &boundingBox() const override;

        private:
            /**
             * 计算击中点的法向量
             * @param hitPoint
             * @return
             */
            virtual Vector3 computeNormal(const Vector3 &hitPoint);

        private:
            AABB _aabb;
            Vector3 _center;
            Vector3 _transformedCenter;
            double _radius;
            bool _outward;
            std::shared_ptr<Material> _material = nullptr;
            std::shared_ptr<Matrix4> _transformMatrix = nullptr;
            std::shared_ptr<Matrix4> _inverseTransformMatrix = nullptr;
        };

    }
}

#endif //KAGUYA_SPHERE_H
