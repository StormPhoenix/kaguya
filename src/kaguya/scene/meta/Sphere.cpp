//
// Created by Storm Phoenix on 2020/10/2.
//

#include <kaguya/scene/meta/Sphere.h>

namespace kaguya {
    namespace scene {

        Sphere::Sphere(const Vector3 &center, double radius, std::shared_ptr<Material> material, bool outward,
                       std::shared_ptr<Matrix4> transformMatrix) {
            _center = center;
            _transformedCenter =
                    transformMatrix != nullptr ? Vector3((*transformMatrix) * Vector4(_center, 1.0f)) : _center;
            _radius = radius;
            _material = material;
            _outward = outward;
            _transformMatrix = transformMatrix;

            // build bounding box
            _aabb = AABB(_transformedCenter - Vector3(_radius, _radius, _radius),
                         _transformedCenter + Vector3(_radius, _radius, _radius));
        }

        Vector3 Sphere::computeNormal(const Vector3 &hitPoint) {
            if (_outward) {
                return NORMALIZE(hitPoint - _transformedCenter);
            } else {
                return NORMALIZE(_transformedCenter - hitPoint);
            }
        }

        bool Sphere::insect(const Ray &ray, SurfaceInteraction &hitRecord, double stepMin, double stepMax) {
            Vector3 centerToOrigin = ray.getOrigin() - _transformedCenter;
            double c = DOT(centerToOrigin, centerToOrigin) - _radius * _radius;
            double a = pow(LENGTH(ray.getDirection()), 2);
            double halfB = DOT(centerToOrigin, ray.getDirection());

            double discriminant = halfB * halfB - a * c;

            // 判别一元二次方程的根数量
            if (discriminant > 0) {
                double root = (-halfB - sqrt(discriminant)) / a;
                if (root >= stepMin && root <= stepMax) {
                    hitRecord.setId(getId());
                    hitRecord.setStep(root);
                    hitRecord.setPoint(ray.at(root));
                    hitRecord.setMaterial(_material.get());
                    hitRecord.setU(0);
                    hitRecord.setV(0);
                    hitRecord.setAreaLight(nullptr);

                    Vector3 outwardNormal = computeNormal(hitRecord.getPoint());
                    hitRecord.setOutwardNormal(outwardNormal, ray.getDirection());
                    return true;
                }

                root = (-halfB + sqrt(discriminant)) / a;
                if (root >= stepMin && root <= stepMax) {
                    hitRecord.setId(getId());
                    hitRecord.setStep( root);
                    hitRecord.setPoint(ray.at(root));
                    hitRecord.setMaterial( _material.get());
                    hitRecord.setU(0);
                    hitRecord.setV(0);
                    hitRecord.setAreaLight(nullptr);

                    Vector3 outwardNormal = computeNormal(hitRecord.getPoint());
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