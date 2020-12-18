//
// Created by Storm Phoenix on 2020/10/2.
//

#include <kaguya/scene/meta/Triangle.h>

namespace kaguya {
    namespace scene {

        Triangle::Triangle(const Vertex &a, const Vertex &b, const Vertex &c,
                           std::shared_ptr<Matrix4> transformMatrix)
                : _position1(a.position), _position2(b.position), _position3(c.position),
                  _normal1(a.normal), _normal2(b.normal), _normal3(c.normal),
                  _uv1(Vector2(a.u, a.v)), _uv2(Vector2(b.u, b.v)), _uv3(Vector2(c.u, c.v)),
                  _transformMatrix(transformMatrix) {
            init();
        }

        Triangle::Triangle(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &normal1,
                           const Vector3 &normal2, const Vector3 &normal3, const Vector2 &uv1, const Vector2 &uv2,
                           const Vector2 &uv3,
                           std::shared_ptr<Matrix4> transformMatrix)
                : _position1(a), _position2(b), _position3(c),
                  _normal1(normal1), _normal2(normal2), _normal3(normal3),
                  _uv1(uv1), _uv2(uv2), _uv3(uv3),
                  _transformMatrix(transformMatrix) {
            init();
        }

        void Triangle::init() {
            // 创建 AABB
            _transformedPosition1 = _transformMatrix != nullptr ?
                                    (*_transformMatrix) * Vector4(_position1, 1.0f) : _position1;
            _transformedPosition2 = _transformMatrix != nullptr ?
                                    (*_transformMatrix) * Vector4(_position2, 1.0f) : _position2;
            _transformedPosition3 = _transformMatrix != nullptr ?
                                    (*_transformMatrix) * Vector4(_position3, 1.0f) : _position3;

            // 计算变换后的法线，参考 https://blog.csdn.net/lawest/article/details/98328127
            _transformedNormal1 = _transformMatrix != nullptr ?
                                  NORMALIZE(Vector3(INVERSE_TRANSPOSE(*_transformMatrix) * Vector4(_normal1, 0.0f))) :
                                  NORMALIZE(_normal1);
            _transformedNormal2 = _transformMatrix != nullptr ?
                                  NORMALIZE(Vector3(INVERSE_TRANSPOSE(*_transformMatrix) * Vector4(_normal2, 0.0f))) :
                                  NORMALIZE(_normal2);
            _transformedNormal3 = _transformMatrix != nullptr ?
                                  NORMALIZE(Vector3(INVERSE_TRANSPOSE(*_transformMatrix) * Vector4(_normal3, 0.0f))) :
                                  NORMALIZE(_normal3);


            double minX =
                    std::min(
                            std::min(_transformedPosition1[0], _transformedPosition2[0]),
                            _transformedPosition3[0]
                    ) - 0.0001;
            double minY =
                    std::min(
                            std::min(_transformedPosition1[1], _transformedPosition2[1]),
                            _transformedPosition3[1]
                    ) - 0.0001;
            double minZ =
                    std::min(
                            std::min(_transformedPosition1[2], _transformedPosition2[2]),
                            _transformedPosition3[2]
                    ) - 0.0001;

            double maxX =
                    std::max(
                            std::max(_transformedPosition1[0], _transformedPosition2[0]),
                            _transformedPosition3[0]
                    ) + 0.0001;
            double maxY =
                    std::max(
                            std::max(_transformedPosition1[1], _transformedPosition2[1]),
                            _transformedPosition3[1]
                    ) + 0.0001;
            double maxZ =
                    std::max(
                            std::max(_transformedPosition1[2], _transformedPosition2[2]),
                            _transformedPosition3[2]
                    ) + 0.0001;

            _aabb = AABB(Vector3(minX, minY, minZ), Vector3(maxX, maxY, maxZ));
        }

        bool Triangle::insect(Ray &ray, SurfaceInteraction &si,
                              double stepMin, double stepMax) const {

            const Vector3 &dir = ray.getDirection();
            const Vector3 &eye = Vector3(ray.getOrigin().x, ray.getOrigin().y, ray.getOrigin().z);
            Matrix3 equationParam(Vector3(_transformedPosition1 - _transformedPosition2),
                                  Vector3(_transformedPosition1 - _transformedPosition3),
                                  dir);

            if (abs(DETERMINANT(equationParam)) < EPSILON) {
                return false;
            }

            Vector3 equationResult = _transformedPosition1 - eye;
            Vector3 ans = INVERSE(equationParam) * equationResult;
            double step = ans[2];
            double alpha = 1 - ans[0] - ans[1];

            // 检查射线范围、击中点是否在三角形内部
            if (step < stepMax &&
                step > stepMin &&
                checkRange(alpha, 0, 1) &&
                checkRange(ans[0], 0, 1) &&
                checkRange(ans[1], 0, 1)) {

                Vector3 factor = Vector3(alpha, ans[0], ans[1]);

                ray.setStep(step);
                si.setPoint(ray.at(step));

                Vector3 normal = alpha * _transformedNormal1 +
                                 ans[0] * _transformedNormal2 +
                                 ans[1] * _transformedNormal3;

                si.setId(getId());
                si.setOutwardNormal(normal, dir);
                si.setU(DOT(factor, Vector3(_uv1.x, _uv2.x, _uv3.x)));
                si.setV(DOT(factor, Vector3(_uv1.y, _uv2.y, _uv3.y)));
                si.setAreaLight(nullptr);
                return true;
            } else {
                return false;
            }
        }

        double Triangle::area() const {
            return 0.5 * LENGTH(CROSS((_transformedPosition2 - _transformedNormal1),
                                      (_transformedNormal3 - _transformedNormal1)));
        }

        SurfaceInteraction Triangle::sampleSurfacePoint(const Sampler1D *sampler1D) const {
            // Uniform sampling triangle
            Vector2 barycentric = triangleUniformSampling(sampler1D);

            SurfaceInteraction si;
            Vector3 p = _transformedPosition1 * barycentric[0] +
                        _transformedPosition2 * barycentric[1] +
                        _transformedPosition3 * (1 - barycentric[0] - barycentric[1]);
            si.setPoint(p);

            // geometry normal
            Vector3 ng = NORMALIZE(CROSS(_transformedPosition2 - _transformedPosition1,
                                         _transformedPosition3 - _transformedPosition1));

            // shading normal
            Vector3 ns = _transformedNormal1 * barycentric[0] +
                         _transformedNormal2 * barycentric[1] +
                         _transformedNormal3 * (1 - barycentric[0] - barycentric[1]);

            // correct geometry normal
            if (DOT(ng, ns) < 0) {
                ng *= -1;
            }
            si.setNormal(ng);
            return si;
        }

        const AABB &Triangle::boundingBox() const {
            return _aabb;
        }

    }
}