//
// Created by Storm Phoenix on 2020/10/2.
//

#include <kaguya/scene/meta/Sphere.h>

namespace kaguya {
    namespace scene {

        Sphere::Sphere(const Vector3 &center, double radius, std::shared_ptr<Material> material,
                       std::shared_ptr<Matrix4> transformMatrix) {
            _center = center;
            _transformedCenter = transformMatrix != nullptr ? Vector3((*transformMatrix) * Vector4(_center, 1.0f)) : _center;
            _radius = radius;
            _material = material;
            _transformMatrix = transformMatrix;

            // build bounding box
            _aabb = AABB(_transformedCenter - Vector3(_radius, _radius, _radius),
                         _transformedCenter + Vector3(_radius, _radius, _radius));
        }

        bool Sphere::hit(const Ray &ray, HitRecord &hitRecord, double stepMin, double stepMax) {
            Vector3 centerToOrigin = ray.getOrigin() - _transformedCenter;
            double c = DOT(centerToOrigin, centerToOrigin) - _radius * _radius;
            double a = pow(LENGTH(ray.getDirection()), 2);
            double halfB = DOT(centerToOrigin, ray.getDirection());

            double discriminant = halfB * halfB - a * c;

            // 判别一元二次方程的根数量
            if (discriminant > 0) {
                double root = (-halfB - sqrt(discriminant)) / a;
                if (root >= stepMin && root <= stepMax) {
                    hitRecord.step = root;
                    hitRecord.point = ray.at(root);
                    hitRecord.material = _material;
                    hitRecord.u = 0;
                    hitRecord.v = 0;

                    Vector3 outwardNormal = hitRecord.point - _transformedCenter;
                    hitRecord.setOutwardNormal(outwardNormal, ray.getDirection());
                    return true;
                }

                root = (-halfB + sqrt(discriminant)) / a;
                if (root >= stepMin && root <= stepMax) {
                    hitRecord.step = root;
                    hitRecord.point = ray.at(root);
                    hitRecord.material = _material;

                    Vector3 outwardNormal = hitRecord.point - _transformedCenter;
                    hitRecord.setOutwardNormal(outwardNormal, ray.getDirection());
                    return true;
                } else {
                    return false;
                }
            } else {
                // no root
                return false;
            }
        }

        const AABB &Sphere::boundingBox() const {
            return _aabb;
        }
    }
}