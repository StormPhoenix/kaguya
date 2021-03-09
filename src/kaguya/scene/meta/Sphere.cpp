//
// Created by Storm Phoenix on 2020/10/2.
//

#include <kaguya/scene/meta/Sphere.h>

namespace kaguya {
    namespace scene {
        namespace meta {
            Sphere::Sphere(const Vector3F &center, Float radius, bool outward,
                           std::shared_ptr<Transform> transformMatrix) {
                assert(radius > 0);

                _center = center;
                _transformedCenter = transformMatrix->transformPoint(_center);
                _radius = radius;
                _outward = outward;
                _transformMatrix = transformMatrix;

                // build bounding box
                _aabb = AABB(_transformedCenter - Vector3F(_radius, _radius, _radius),
                             _transformedCenter + Vector3F(_radius, _radius, _radius));
            }

            Vector3F Sphere::computeNormal(const Vector3F &hitPoint) const {
                if (_outward) {
                    return NORMALIZE(hitPoint - _transformedCenter);
                } else {
                    return NORMALIZE(_transformedCenter - hitPoint);
                }
            }

            bool Sphere::intersect(Ray &ray, SurfaceInteraction &si, Float minStep, Float maxStep) const {
                Vector3F centerToOrigin = ray.getOrigin() - _transformedCenter;
                Float c = DOT(centerToOrigin, centerToOrigin) - _radius * _radius;
                Float a = pow(LENGTH(ray.getDirection()), 2);
                Float halfB = DOT(centerToOrigin, ray.getDirection());

                Float discriminant = halfB * halfB - a * c;

                // 判别一元二次方程的根数量
                if (discriminant > 0) {
                    Float root = (-halfB - sqrt(discriminant)) / a;
                    if (root > minStep && root < maxStep) {
                        Vector3F origin = ray.at(root);
                        origin = (origin - _center) * (_radius / LENGTH(origin - _center)) + _center;
                        si.point = origin;
                        si.u = 0;
                        si.v = 0;
                        si.setAreaLight(nullptr);

                        Vector3F error = math::gamma(5) * ABS(origin);
                        si.error = error;

                        ray.setStep(root);

                        Vector3F outwardNormal = computeNormal(si.point);
                        si.normal = si.rendering.normal = outwardNormal;
                        si.direction = ray.getDirection();
                        si.wo = -NORMALIZE(si.direction);
                        return true;
                    }

                    root = (-halfB + sqrt(discriminant)) / a;
                    if (root > minStep && root < maxStep) {
                        Vector3F origin = ray.at(root);
                        origin = (origin - _center) * (_radius / LENGTH(origin - _center)) + _center;
                        si.point = origin;
                        si.u = 0;
                        si.v = 0;
                        si.setAreaLight(nullptr);
                        Vector3F error = math::gamma(5) * ABS(origin);
                        si.error = error;
                        ray.setStep(root);

                        Vector3F outwardNormal = computeNormal(si.point);
                        si.normal = si.rendering.normal = outwardNormal;
                        si.direction = ray.getDirection();
                        si.wo = -NORMALIZE(si.direction);
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    // no root
                    return false;
                }
            }

            SurfaceInteraction Sphere::sampleSurfacePoint(Sampler *sampler) const {
                // sample1d outward normal
                Vector3F normal = math::sampling::sphereUniformSampling(sampler);
                if (_transformMatrix != nullptr) {
                    normal = _transformMatrix->transformNormal(normal);
                }

                SurfaceInteraction si;

                // point
                Vector3F point = _transformedCenter + _radius * normal;
                si.point = point;

                if (!_outward) {
                    normal = -normal;
                }
                si.normal = si.rendering.normal = normal;
                return si;
            }

            SurfaceInteraction
            Sphere::sampleSurfaceInteraction(const Interaction &eye, Sampler *sampler) const {
                // If eye is inside sphere, then uniform sampling surface
                const Float dist = LENGTH(_transformedCenter - eye.point);
                if (_radius >= dist) {
                    // inside
                    return sampleSurfacePoint(sampler);
                } else {
                    // outside sphere, then use uniform cone sampling strategy

                    // cosThetaMax
                    Float sinThetaMax2 = (_radius * _radius) / (dist * dist);
                    Float cosThetaMax = std::sqrt(std::max(Float(0.), 1 - sinThetaMax2));

                    // cone sampling
                    const Vector3F dir = math::sampling::coneUniformSampling(cosThetaMax, sampler);

                    // build coordinate system
                    Vector3F tanY = NORMALIZE(_transformedCenter - eye.point);
                    Vector3F tanX, tanZ;
                    math::tangentSpace(tanY, &tanX, &tanZ);

                    // 计算世界坐标系中射线方向
                    Vector3F dirWorld = dir.x * tanX + dir.y * tanY + dir.z * tanZ;

                    SurfaceInteraction si;
                    Ray sampleRay = Ray(eye.point, NORMALIZE(dirWorld));
                    bool isInsect = intersect(sampleRay, si, sampleRay.getMinStep(), sampleRay.getStep());
                    assert(isInsect);

                    return si;
                }
            }

            Float Sphere::surfaceInteractionPdf(const Interaction &eye, const Vector3F &dir) const {
                Ray ray = eye.sendRay(dir);

                // get interaction
                SurfaceInteraction si;
                bool foundIntersection = intersect(ray, si, ray.getMinStep(), ray.getStep());
                if (!foundIntersection) {
                    return 0;
                }

                Float pdf = 0;
                Float dist = LENGTH(eye.point - _transformedCenter);
                if (_radius > dist) {
                    // inside sphere
                    Float cosine = ABS_DOT(dir, si.normal);
                    pdf = (dist * dist / area()) / cosine;
                    if (std::isinf(pdf)) {
                        pdf = 0.;
                    }
                } else {
                    // outside
                    Float sinThetaMax2 = (_radius * _radius) / (dist * dist);
                    Float cosThetaMax = std::sqrt(std::max(Float(0.), 1 - sinThetaMax2));

                    pdf = math::sampling::coneUniformSamplePdf(cosThetaMax);
                }
                return pdf;
            }

            const AABB &Sphere::bound() const {
                return _aabb;
            }
        }
    }
}