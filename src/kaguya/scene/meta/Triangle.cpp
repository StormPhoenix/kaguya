//
// Created by Storm Phoenix on 2020/10/2.
//
#include <kaguya/Config.h>
#include <kaguya/scene/meta/Triangle.h>

#include <iostream>

namespace RENDER_NAMESPACE {
    namespace scene {
        namespace meta {
            Triangle::Triangle(const Vector3F &a, const Vector3F &b, const Vector3F &c,
                               const Vector3F &normal1, const Vector3F &normal2, const Vector3F &normal3,
                               const Vector2F &uv1, const Vector2F &uv2, const Vector2F &uv3,
                               std::shared_ptr<Transform> transformMatrix)
                    : _p1(a), _p2(b), _p3(c),
                      _n1(normal1), _n2(normal2), _n3(normal3),
                      _uv1(uv1), _uv2(uv2), _uv3(uv3),
                      _transformMatrix(transformMatrix) {
                assert(transformMatrix != nullptr);
                usingNg = (LENGTH(_n1) == 0 || LENGTH(_n2) == 0 || LENGTH(_n3) == 0);
                init();
            }

            void Triangle::init() {
                // 创建 AABB
                _p1World = _transformMatrix->transformPoint(_p1);
                _p2World = _transformMatrix->transformPoint(_p2);
                _p3World = _transformMatrix->transformPoint(_p3);

                // 计算变换后的法线，参考 https://blog.csdn.net/lawest/article/details/98328127
                if (!usingNg) {
                    _n1World = _transformMatrix->transformNormal(_n1);
                    _n2World = _transformMatrix->transformNormal(_n2);
                    _n3World = _transformMatrix->transformNormal(_n3);
                }

                Float minX =
                        std::min(
                                std::min(_p1World[0], _p2World[0]),
                                _p3World[0]
                        ) - 0.0001;
                Float minY =
                        std::min(
                                std::min(_p1World[1], _p2World[1]),
                                _p3World[1]
                        ) - 0.0001;
                Float minZ =
                        std::min(
                                std::min(_p1World[2], _p2World[2]),
                                _p3World[2]
                        ) - 0.0001;

                Float maxX =
                        std::max(
                                std::max(_p1World[0], _p2World[0]),
                                _p3World[0]
                        ) + 0.0001;
                Float maxY =
                        std::max(
                                std::max(_p1World[1], _p2World[1]),
                                _p3World[1]
                        ) + 0.0001;
                Float maxZ =
                        std::max(
                                std::max(_p1World[2], _p2World[2]),
                                _p3World[2]
                        ) + 0.0001;

                _aabb = AABB(Vector3F(minX, minY, minZ), Vector3F(maxX, maxY, maxZ));
            }

            bool Triangle::intersect(Ray &ray, SurfaceInteraction &si,
                                     Float minStep, Float maxStep) const {
                /* transform the ray direction, let it to point to z-axis */

                // move ray's origin to (0, 0, 0)
                Vector3F p0 = _p1World - ray.getOrigin();
                Vector3F p1 = _p2World - ray.getOrigin();
                Vector3F p2 = _p3World - ray.getOrigin();

                // swap axis
                int zAxis = math::maxAbsAxis(ray.getDirection());
                int xAxis = zAxis + 1 == 3 ? 0 : zAxis + 1;
                int yAxis = xAxis + 1 == 3 ? 0 : xAxis + 1;

                Vector3F dir = math::swapAxis(ray.getDirection(), xAxis, yAxis, zAxis);
                p0 = math::swapAxis(p0, xAxis, yAxis, zAxis);
                p1 = math::swapAxis(p1, xAxis, yAxis, zAxis);
                p2 = math::swapAxis(p2, xAxis, yAxis, zAxis);

                // shear direction to z-axis
                Float shearX = -dir[0] / dir[2];
                Float shearY = -dir[1] / dir[2];

                // transform points to z-axis
                p0[0] += shearX * p0[2];
                p0[1] += shearY * p0[2];
                p1[0] += shearX * p1[2];
                p1[1] += shearY * p1[2];
                p2[0] += shearX * p2[2];
                p2[1] += shearY * p2[2];

                // calculate barycentric
                Float e0 = p1.y * p0.x - p1.x * p0.y;
                Float e1 = p2.y * p1.x - p2.x * p1.y;
                Float e2 = p0.y * p2.x - p0.x * p2.y;

                // check double precision if barycentric is zero
                // Fall back to double precision test at triangle edges
                if (sizeof(Float) == sizeof(float) &&
                    (e0 == 0.0f || e1 == 0.0f || e2 == 0.0f)) {
                    double p1yp0x = (double) p1.y * (double) p0.x;
                    double p1xp0y = (double) p1.x * (double) p0.y;
                    e0 = (float) (p1yp0x - p1xp0y);
                    double p2yp1x = (double) p2.y * (double) p1.x;
                    double p2xp1y = (double) p2.x * (double) p1.y;
                    e1 = (float) (p2yp1x - p2xp1y);
                    double p0yp2x = (double) p0.y * (double) p2.x;
                    double p0xp2y = (double) p0.x * (double) p2.y;
                    e2 = (float) (p0yp2x - p0xp2y);
                }

                if ((e0 > 0 || e1 > 0 || e2 > 0) && (e0 < 0 || e1 < 0 || e2 < 0)) {
                    // intersection point doesn't fall in triangle area.
                    return false;
                }

                Float sum = e0 + e1 + e2;

                if (sum == 0) {
                    return false;
                }

                // interpolate step * sum
                Float shearZ = 1 / dir[2];
                p0[2] *= shearZ;
                p1[2] *= shearZ;
                p2[2] *= shearZ;
                Float sumMulStep = e0 * p2.z + e1 * p0.z + e2 * p1.z;

                // make sure step > 0 and step < t_max
                if (sum > 0 && (sumMulStep <= sum * minStep || sumMulStep >= sum * maxStep)) {
                    return false;
                } else if (sum < 0 && (sumMulStep >= sum * minStep || sumMulStep <= sum * maxStep)) {
                    return false;
                }

                Float invSum = 1. / sum;
                Float b0 = e0 * invSum;
                Float b1 = e1 * invSum;
                Float b2 = e2 * invSum;
                Float step = sumMulStep * invSum;

                Float maxZ = math::maxAbsAxisValue(Vector3F(p0.z, p1.z, p2.z));
                Float deltaZ = math::gamma(3) * maxZ;

                Float maxX = math::maxAbsAxisValue(Vector3F(p0.x, p1.x, p2.x));
                Float maxY = math::maxAbsAxisValue(Vector3F(p0.y, p1.y, p2.y));

                Float deltaX = math::gamma(5) * (maxX + maxZ);
                Float deltaY = math::gamma(5) * (maxY + maxZ);

                Float deltaE = 2 * (math::gamma(2) * maxX * maxY + deltaY * maxX + deltaX * maxY);

                Float maxE = math::maxAbsAxisValue(Vector3F(e0, e1, e2));
                Float deltaT = 3 * (math::gamma(3) * maxE * maxZ + deltaE * maxZ + deltaZ * maxE) * std::abs(invSum);
                if (step <= deltaT) {
                    return false;
                }

                // calculate float-error
                Float zError = math::gamma(7) * (std::abs(b0 * p2.z) + std::abs(b1 * p0.z) + std::abs(b2 * p1.z));
                Float xError = math::gamma(7) * (std::abs(b0 * p2.x) + std::abs(b1 * p0.x) + std::abs(b2 * p1.x));
                Float yError = math::gamma(7) * (std::abs(b0 * p2.y) + std::abs(b1 * p0.y) + std::abs(b2 * p1.y));
                Vector3F error = Vector3F(xError, yError, zError);

                si.point = b1 * _p1World + b2 * _p2World + b0 * _p3World;

                Vector3F ng, ns;
                ng = NORMALIZE(CROSS(_p2World - _p1World, _p3World - _p1World));
                if (usingNg) {
                    ns = ng;
                } else {
                    ns = b1 * _n1World + b2 * _n2World + b0 * _n3World;
                    if (DOT(ns, ng) < 0) {
                        ng *= -1;
                    }
                    if (Config::Tracer::strictNormals) {
                        ns = ng;
                    }
                }
                si.normal = ng;
                si.rendering.normal = ns;

                si.direction = ray.getDirection();
                si.wo = -NORMALIZE(si.direction);
                si.u = _uv1.x * b1 + _uv2.x * b2 + _uv3.x * b0;
                si.v = _uv1.y * b1 + _uv2.y * b2 + _uv3.y * b0;
                si.error = error;
                si.setAreaLight(nullptr);
                ray.setStep(step);
                return true;
            }

            Float Triangle::area() const {
                // TODO 预先计算
                return 0.5 * LENGTH(CROSS((_p2World - _p1World),
                                          (_p3World - _p1World)));
            }

            SurfaceInteraction Triangle::sampleSurfacePoint(Float *pdf, Sampler *sampler) const {
                // Uniform sampling triangle
                Vector2F barycentric = math::sampling::triangleUniformSampling(sampler);

                SurfaceInteraction si;
                Vector3F p = _p1World * barycentric[0] +
                             _p2World * barycentric[1] +
                             _p3World * (1 - barycentric[0] - barycentric[1]);
                si.point = p;

                // error bound
                Point3F pAbsSum = ABS(barycentric[0] * _p1World) +
                                  ABS(barycentric[1] * _p2World) +
                                  ABS((1 - barycentric[0] - barycentric[1]) * _p3World);
                si.error = math::gamma(6) * Vector3F(pAbsSum.x, pAbsSum.y, pAbsSum.z);

                // geometry normal
                Vector3F ng = NORMALIZE(CROSS(_p2World - _p1World,
                                              _p3World - _p1World));

                // shading normal
                Vector3F ns(0);
                if (usingNg) {
                    ns = ng;
                } else {
                    ns = _n1World * barycentric[0] +
                         _n2World * barycentric[1] +
                         _n3World * (1 - barycentric[0] - barycentric[1]);

                    if (DOT(ns, ng) < 0) {
                        // correct geometry normal
                        ng *= -1;
                    }
                }

                if (pdf != nullptr) {
                    (*pdf) = 1. / area();
                }

                si.normal = ng;
                si.rendering.normal = ns;
                return si;
            }

            const AABB &Triangle::bound() const {
                return _aabb;
            }
        }
    }
}