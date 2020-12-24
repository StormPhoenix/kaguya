//
// Created by Storm Phoenix on 2020/10/2.
//

#include <kaguya/scene/meta/Sphere.h>

namespace kaguya {
    namespace scene {
        namespace meta {
            Sphere::Sphere(const Vector3 &center, double radius, bool outward,
                           std::shared_ptr<Matrix4> transformMatrix) {
                assert(radius > 0);

                _center = center;
                _transformedCenter =
                        transformMatrix != nullptr ? Vector3((*transformMatrix) * Vector4(_center, 1.0f)) : _center;
                _radius = radius;
                _outward = outward;
                _transformMatrix = transformMatrix;

                // build bounding box
                _aabb = AABB(_transformedCenter - Vector3(_radius, _radius, _radius),
                             _transformedCenter + Vector3(_radius, _radius, _radius));
            }

            Vector3 Sphere::computeNormal(const Vector3 &hitPoint) const {
                if (_outward) {
                    return NORMALIZE(hitPoint - _transformedCenter);
                } else {
                    return NORMALIZE(_transformedCenter - hitPoint);
                }
            }

            bool Sphere::intersect(Ray &ray, SurfaceInteraction &si, double minStep, double maxStep) const {
                Vector3 centerToOrigin = ray.getOrigin() - _transformedCenter;
                double c = DOT(centerToOrigin, centerToOrigin) - _radius * _radius;
                double a = pow(LENGTH(ray.getDirection()), 2);
                double halfB = DOT(centerToOrigin, ray.getDirection());

                double discriminant = halfB * halfB - a * c;

                // 判别一元二次方程的根数量
                if (discriminant > 0) {
                    double root = (-halfB - sqrt(discriminant)) / a;
                    if (root > minStep && root < maxStep) {
                        Vector3 origin = ray.at(root);
                        origin *= _radius / LENGTH(origin - _center);
                        si.setPoint(origin);
                        si.setU(0);
                        si.setV(0);
                        si.setAreaLight(nullptr);

                        Vector3 error = gamma(5) * ABS(origin);
                        si.setError(error);

                        ray.setStep(root);

                        Vector3 outwardNormal = computeNormal(si.getPoint());
                        si.setOutwardNormal(outwardNormal, ray.getDirection());
                        return true;
                    }

                    root = (-halfB + sqrt(discriminant)) / a;
                    if (root > minStep && root < maxStep) {
                        Vector3 origin = ray.at(root);
                        origin *= _radius / LENGTH(origin - _center);
                        si.setPoint(origin);
                        si.setU(0);
                        si.setV(0);
                        si.setAreaLight(nullptr);
                        Vector3 error = gamma(5) * ABS(origin);
                        si.setError(error);
                        ray.setStep(root);

                        Vector3 outwardNormal = computeNormal(si.getPoint());
                        si.setOutwardNormal(outwardNormal, ray.getDirection());
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    // no root
                    return false;
                }
            }

            SurfaceInteraction Sphere::sampleSurfacePoint(const Sampler1D *sampler1D) const {
                // sample outward normal
                Vector3 normal = sphereUniformSampling(sampler1D);
                if (_transformMatrix != nullptr) {
                    normal = NORMALIZE(Vector3(INVERSE_TRANSPOSE(*_transformMatrix) * Vector4(normal, 0.0f)));
                }

                SurfaceInteraction si;

                // point
                Vector3 point = _transformedCenter + _radius * normal;
                si.setPoint(point);

                if (!_outward) {
                    normal = -normal;
                }
                si.setNormal(normal);
                return si;
            }

            SurfaceInteraction
            Sphere::sampleSurfaceInteraction(const Interaction &eye, const Sampler1D *sampler1D) const {
                // If eye is inside sphere, then uniform sampling surface
                const double dist = LENGTH(_transformedCenter - eye.getPoint());
                if (_radius >= dist) {
                    // inside
                    return sampleSurfacePoint(sampler1D);
                } else {
                    // outside sphere, then use uniform cone sampling strategy

                    // cosThetaMax
                    double sinThetaMax2 = (_radius * _radius) / (dist * dist);
                    double cosThetaMax = std::sqrt(std::max(0., 1 - sinThetaMax2));

                    // cone sampling
                    const Vector3 dir = coneUniformSampling(cosThetaMax, sampler1D);

                    // build coordinate system
                    Vector3 tanY = NORMALIZE(_transformedCenter - eye.getPoint());
                    Vector3 tanX, tanZ;
                    tangentSpace(tanY, &tanX, &tanZ);

                    // 计算世界坐标系中射线方向
                    Vector3 dirWorld = dir.x * tanX + dir.y * tanY + dir.z * tanZ;

                    SurfaceInteraction si;
                    Ray sampleRay = Ray(eye.getPoint(), NORMALIZE(dirWorld));
                    bool isInsect = intersect(sampleRay, si, sampleRay.getMinStep(), sampleRay.getStep());
                    assert(isInsect);

                    return si;
                }
            }

            double Sphere::surfaceInteractionPdf(const Interaction &eye, const Vector3 &dir) const {
                Ray ray = eye.sendRay(dir);

                // get interaction
                SurfaceInteraction si;
                bool foundIntersection = intersect(ray, si, ray.getMinStep(), ray.getStep());
                if (!foundIntersection) {
                    return 0;
                }

                double pdf = 0;
                double dist = LENGTH(eye.getPoint() - _transformedCenter);
                if (_radius > dist) {
                    // inside sphere
                    double cosine = ABS_DOT(dir, si.getNormal());
                    pdf = (dist * dist / area()) / cosine;
                    if (std::isinf(pdf)) {
                        pdf = 0.;
                    }
                } else {
                    // outside
                    double sinThetaMax2 = (_radius * _radius) / (dist * dist);
                    double cosThetaMax = std::sqrt(std::max(0., 1 - sinThetaMax2));

                    pdf = coneUniformSamplePdf(cosThetaMax);
                }
                return pdf;
            }

            const AABB &Sphere::bound() const {
                return _aabb;
            }
        }
    }
}