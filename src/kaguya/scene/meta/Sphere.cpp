//
// Created by Storm Phoenix on 2020/10/2.
//

#include <kaguya/scene/meta/Sphere.h>

namespace kaguya {
    namespace scene {
        namespace meta {
            Sphere::Sphere(const Vector3d &center, double radius, bool outward,
                           std::shared_ptr<Transform> transformMatrix) {
                assert(radius > 0);

                _center = center;
                _transformedCenter = transformMatrix->transformPoint(_center);
                _radius = radius;
                _outward = outward;
                _transformMatrix = transformMatrix;

                // build bounding box
                _aabb = AABB(_transformedCenter - Vector3d(_radius, _radius, _radius),
                             _transformedCenter + Vector3d(_radius, _radius, _radius));
            }

            Vector3d Sphere::computeNormal(const Vector3d &hitPoint) const {
                if (_outward) {
                    return NORMALIZE(hitPoint - _transformedCenter);
                } else {
                    return NORMALIZE(_transformedCenter - hitPoint);
                }
            }

            bool Sphere::intersect(Ray &ray, SurfaceInteraction &si, double minStep, double maxStep) const {
                Vector3d centerToOrigin = ray.getOrigin() - _transformedCenter;
                double c = DOT(centerToOrigin, centerToOrigin) - _radius * _radius;
                double a = pow(LENGTH(ray.getDirection()), 2);
                double halfB = DOT(centerToOrigin, ray.getDirection());

                double discriminant = halfB * halfB - a * c;

                // 判别一元二次方程的根数量
                if (discriminant > 0) {
                    double root = (-halfB - sqrt(discriminant)) / a;
                    if (root > minStep && root < maxStep) {
                        Vector3d origin = ray.at(root);
                        origin *= _radius / LENGTH(origin - _center);
                        si.setPoint(origin);
                        si.setU(0);
                        si.setV(0);
                        si.setAreaLight(nullptr);

                        Vector3d error = math::gamma(5) * ABS(origin);
                        si.setError(error);

                        ray.setStep(root);

                        Vector3d outwardNormal = computeNormal(si.getPoint());
                        si.setOutwardNormal(outwardNormal, ray.getDirection());
                        return true;
                    }

                    root = (-halfB + sqrt(discriminant)) / a;
                    if (root > minStep && root < maxStep) {
                        Vector3d origin = ray.at(root);
                        origin *= _radius / LENGTH(origin - _center);
                        si.setPoint(origin);
                        si.setU(0);
                        si.setV(0);
                        si.setAreaLight(nullptr);
                        Vector3d error = math::gamma(5) * ABS(origin);
                        si.setError(error);
                        ray.setStep(root);

                        Vector3d outwardNormal = computeNormal(si.getPoint());
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

            SurfaceInteraction Sphere::sampleSurfacePoint(Sampler *sampler1D) const {
                // sample1d outward normal
                Vector3d normal = math::sampling::sphereUniformSampling(sampler1D);
                if (_transformMatrix != nullptr) {
                    normal = _transformMatrix->transformNormal(normal);
                }

                SurfaceInteraction si;

                // point
                Vector3d point = _transformedCenter + _radius * normal;
                si.setPoint(point);

                if (!_outward) {
                    normal = -normal;
                }
                si.setNormal(normal);
                return si;
            }

            SurfaceInteraction
            Sphere::sampleSurfaceInteraction(const Interaction &eye, Sampler *sampler1D) const {
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
                    const Vector3d dir = math::sampling::coneUniformSampling(cosThetaMax, sampler1D);

                    // build coordinate system
                    Vector3d tanY = NORMALIZE(_transformedCenter - eye.getPoint());
                    Vector3d tanX, tanZ;
                    math::tangentSpace(tanY, &tanX, &tanZ);

                    // 计算世界坐标系中射线方向
                    Vector3d dirWorld = dir.x * tanX + dir.y * tanY + dir.z * tanZ;

                    SurfaceInteraction si;
                    Ray sampleRay = Ray(eye.getPoint(), NORMALIZE(dirWorld));
                    bool isInsect = intersect(sampleRay, si, sampleRay.getMinStep(), sampleRay.getStep());
                    assert(isInsect);

                    return si;
                }
            }

            double Sphere::surfaceInteractionPdf(const Interaction &eye, const Vector3d &dir) const {
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