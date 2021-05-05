//
// Created by Storm Phoenix on 2020/10/2.
//

#include <kaguya/scene/meta/Sphere.h>

namespace kaguya {
    namespace scene {
        namespace meta {

            Sphere::Sphere(const Vector3F &center, Float radius, bool outward) {
                assert(radius > 0);

                Matrix4F transformMat(1.0f);
                transformMat = TRANSLATE(transformMat, Vector3F(center[0], center[1], center[2]));
                transformMat = SCALE(transformMat, Vector3F(radius, radius, radius));
                _transformMatrix = std::make_shared<Transform>(transformMat);
                _inverseTransformMatrix = _transformMatrix->inverse().ptr();

                _center = Vector3F(0.);
                _transformedCenter = center;
                _radius = radius;
                _outward = outward;

                // build bounding box
                _aabb = AABB(_transformedCenter - Vector3F(_radius, _radius, _radius),
                             _transformedCenter + Vector3F(_radius, _radius, _radius));
            }

            Sphere::Sphere(std::shared_ptr<Transform> transformMatrix, bool outward) {
                _transformMatrix = transformMatrix;
                if (_transformMatrix == nullptr) {
                    _transformMatrix = std::make_shared<Transform>();
                }
                _inverseTransformMatrix = _transformMatrix->inverse().ptr();
                _center = Vector3F(0.);
                _transformedCenter = _transformMatrix->transformPoint(_center);
                _radius = LENGTH(
                        Vector3F(_transformMatrix->transformPoint(Vector3F(1.0, 0.0, 0.0)) - _transformedCenter));
                _outward = outward;
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
                        origin = (origin - _transformedCenter) * (_radius / LENGTH(origin - _transformedCenter)) + _transformedCenter;
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
                        origin = (origin - _transformedCenter) * (_radius / LENGTH(origin - _transformedCenter)) + _transformedCenter;
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

            SurfaceInteraction Sphere::sampleSurfacePoint(Float *pdf, Sampler *sampler) const {
                // Sample outward normal
                Vector3F nLocal = math::sampling::sphereUniformSampling(sampler);

                // World normal
                Vector3F nWorld = _transformMatrix != nullptr ? _transformMatrix->transformNormal(nLocal) : nLocal;

                // Project pLocal to sphere surface to compute error bound
                Vector3F p = nWorld * _radius + _transformedCenter;
                p = (p - _transformedCenter) * (_radius / LENGTH(p - _transformedCenter)) + _transformedCenter;

                SurfaceInteraction si;
                si.point = p;

                Vector3F pError = math::gamma(5) * ABS(p);
                si.error = pError;

                // Reverse orientation
                if (!_outward) {
                    nWorld = -nWorld;
                }
                si.normal = si.rendering.normal = nWorld;
                (*pdf) = 1. / area();
                return si;
            }

            SurfaceInteraction
            Sphere::sampleSurfaceInteraction(const Interaction &eye, Float *pdf, Sampler *sampler) const {
                // If eye is inside sphere, then uniform sampling surface
                const Float eyeSphereDist = LENGTH(_transformedCenter - eye.point);
                if (_radius >= eyeSphereDist) {
                    // inside
                    Float density = 0;
                    SurfaceInteraction si = sampleSurfacePoint(&density, sampler);
                    Vector3F wi = si.point - eye.point;
                    Float dist = LENGTH(wi);
                    if (dist == 0.) {
                        (*pdf) = 0;
                    } else {
                        wi = NORMALIZE(wi);
                        Float cosTheta = ABS_DOT(-wi, si.normal);
                        (*pdf) = density * (dist * dist) / cosTheta;
                        if (std::isinf(*pdf)) {
                            (*pdf) = 0;
                        }
                    }
                    return si;
                } else {
                    // outside sphere, then use uniform cone sampling strategy

                    // cosThetaMax
                    Float sinThetaMax2 = (_radius * _radius) / (eyeSphereDist * eyeSphereDist);
                    Float cosThetaMax = std::sqrt(std::max(Float(0.), 1 - sinThetaMax2));

                    // cone sampling
                    const Vector3F dir = math::sampling::coneUniformSampling(cosThetaMax, sampler);

                    // build coordinate system
                    Vector3F tanY = NORMALIZE(_transformedCenter - eye.point);
                    Vector3F tanX, tanZ;
                    math::tangentSpace(tanY, &tanX, &tanZ);

                    // 计算世界坐标系中射线方向
                    Vector3F dirWorld = dir.x * tanX + dir.y * tanY + dir.z * tanZ;

                    // TODO Stupid code ...
                    SurfaceInteraction si;
                    Ray sampleRay = Ray(eye.point, NORMALIZE(dirWorld));
                    bool isInsect = intersect(sampleRay, si, sampleRay.getMinStep(), sampleRay.getStep());
                    assert(isInsect);

                    if (pdf != nullptr) {
                        (*pdf) = 1 / (2 * math::PI * (1 - cosThetaMax));
                    }

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