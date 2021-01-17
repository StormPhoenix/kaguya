//
// Created by Storm Phoenix on 2020/10/2.
//

#include <kaguya/scene/meta/Triangle.h>

namespace kaguya {
    namespace scene {
        namespace meta {
            Triangle::Triangle(const Vertex &a, const Vertex &b, const Vertex &c,
                               std::shared_ptr<Transform> transformMatrix)
                    : _position1(a.position), _position2(b.position), _position3(c.position),
                      _normal1(a.normal), _normal2(b.normal), _normal3(c.normal),
                      _uv1(Vector2F(a.u, a.v)), _uv2(Vector2F(b.u, b.v)), _uv3(Vector2F(c.u, c.v)),
                      _transformMatrix(transformMatrix) {
                assert(transformMatrix != nullptr);
                init();
            }

            Triangle::Triangle(const Vector3F &a, const Vector3F &b, const Vector3F &c, const Vector3F &normal1,
                               const Vector3F &normal2, const Vector3F &normal3, const Vector2f &uv1,
                               const Vector2f &uv2,
                               const Vector2f &uv3,
                               std::shared_ptr<Transform> transformMatrix)
                    : _position1(a), _position2(b), _position3(c),
                      _normal1(normal1), _normal2(normal2), _normal3(normal3),
                      _uv1(uv1), _uv2(uv2), _uv3(uv3),
                      _transformMatrix(transformMatrix) {
                assert(transformMatrix != nullptr);
                init();
            }

            void Triangle::init() {
                // 创建 AABB
                _transformedPosition1 = _transformMatrix->transformPoint(_position1);
                _transformedPosition2 = _transformMatrix->transformPoint(_position2);
                _transformedPosition3 = _transformMatrix->transformPoint(_position3);

                // 计算变换后的法线，参考 https://blog.csdn.net/lawest/article/details/98328127
                _transformedNormal1 = _transformMatrix->transformNormal(_normal1);
                _transformedNormal2 = _transformMatrix->transformNormal(_normal2);
                _transformedNormal3 = _transformMatrix->transformNormal(_normal3);

                // Calculate geometry normal
                _geometryNormal = NORMALIZE(CROSS(_transformedPosition3 - _transformedPosition2,
                                                  _transformedPosition1 - _transformedPosition2));
                if (DOT(_geometryNormal, _transformedNormal1) < 0) {
                    _geometryNormal = -_geometryNormal;
                }

                Float minX =
                        std::min(
                                std::min(_transformedPosition1[0], _transformedPosition2[0]),
                                _transformedPosition3[0]
                        ) - 0.0001;
                Float minY =
                        std::min(
                                std::min(_transformedPosition1[1], _transformedPosition2[1]),
                                _transformedPosition3[1]
                        ) - 0.0001;
                Float minZ =
                        std::min(
                                std::min(_transformedPosition1[2], _transformedPosition2[2]),
                                _transformedPosition3[2]
                        ) - 0.0001;

                Float maxX =
                        std::max(
                                std::max(_transformedPosition1[0], _transformedPosition2[0]),
                                _transformedPosition3[0]
                        ) + 0.0001;
                Float maxY =
                        std::max(
                                std::max(_transformedPosition1[1], _transformedPosition2[1]),
                                _transformedPosition3[1]
                        ) + 0.0001;
                Float maxZ =
                        std::max(
                                std::max(_transformedPosition1[2], _transformedPosition2[2]),
                                _transformedPosition3[2]
                        ) + 0.0001;

                _aabb = AABB(Vector3F(minX, minY, minZ), Vector3F(maxX, maxY, maxZ));
            }

            bool Triangle::intersect(Ray &ray, SurfaceInteraction &si,
                                     Float minStep, Float maxStep) const {

                /* transform the ray direction, let it to point to z-axis */
                // move ray's origin to (0, 0, 0)
                Vector3F p0 = _transformedPosition1 - ray.getOrigin();
                Vector3F p1 = _transformedPosition2 - ray.getOrigin();
                Vector3F p2 = _transformedPosition3 - ray.getOrigin();

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
                if (sum > 0 && (sumMulStep <= 0 || sumMulStep >= sum * maxStep)) {
                    return false;
                } else if (sum < 0 && (sumMulStep >= 0 || sumMulStep <= sum * maxStep)) {
                    return false;
                }

                Float invSum = 1. / sum;
                Float step = sumMulStep * invSum;
                Float b0 = e0 * invSum;
                Float b1 = e1 * invSum;
                Float b2 = e2 * invSum;

                // calculate float-error
                Float zError = math::gamma(7) * (std::abs(b0 * p2.z) + std::abs(b1 * p0.z) + std::abs(b2 * p1.z));
                Float xError = math::gamma(7) * (std::abs(b0 * p2.x) + std::abs(b1 * p0.x) + std::abs(b2 * p1.x));
                Float yError = math::gamma(7) * (std::abs(b0 * p2.y) + std::abs(b1 * p0.y) + std::abs(b2 * p1.y));
                Vector3F error = Vector3F(xError, yError, zError);

                ray.setStep(step);
                si.point = ray.at(step);

                Vector3F normal = b0 * _transformedNormal3 +
                                  b1 * _transformedNormal1 +
                                  b2 * _transformedNormal2;

                si.normal = _geometryNormal;
                si.rendering.normal = normal;
                si.direction = ray.getDirection();
                si.u = _uv1.x * b1 + _uv2.x * b2 + _uv3.x * b0;
                si.v = _uv1.y * b1 + _uv2.y * b2 + _uv3.y * b0;
                si.error = error;
                si.setAreaLight(nullptr);
                return true;
            }

            Float Triangle::area() const {
                return 0.5 * LENGTH(CROSS((_transformedPosition2 - _transformedPosition1),
                                          (_transformedPosition3 - _transformedPosition1)));
            }

            SurfaceInteraction Triangle::sampleSurfacePoint(Sampler *sampler1D) const {
                // Uniform sampling triangle
                Vector2F barycentric = math::sampling::triangleUniformSampling(sampler1D);

                SurfaceInteraction si;
                Vector3F p = _transformedPosition1 * barycentric[0] +
                             _transformedPosition2 * barycentric[1] +
                             _transformedPosition3 * (1 - barycentric[0] - barycentric[1]);
                si.point = p;

                // geometry normal
                Vector3F ng = NORMALIZE(CROSS(_transformedPosition2 - _transformedPosition1,
                                              _transformedPosition3 - _transformedPosition1));

                // shading normal
                Vector3F ns = _transformedNormal1 * barycentric[0] +
                              _transformedNormal2 * barycentric[1] +
                              _transformedNormal3 * (1 - barycentric[0] - barycentric[1]);

                // correct geometry normal
                if (DOT(ng, ns) < 0) {
                    ng *= -1;
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