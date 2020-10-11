//
// Created by Storm Phoenix on 2020/10/2.
//

#include <kaguya/scene/meta/Triangle.h>

namespace kaguya {
    namespace scene {

        Triangle::Triangle(const Vertex &a, const Vertex &b, const Vertex &c,
                           std::shared_ptr<Material> material, std::shared_ptr<Matrix4> transformMatrix)
                : _position1(a.position), _position2(b.position), _position3(c.position),
                  _normal1(a.normal), _normal2(b.normal), _normal3(c.normal),
                  _uv1(Vector2(a.u, a.v)), _uv2(Vector2(b.u, b.v)), _uv3(Vector2(c.u, c.v)),
                  _material(material), _transformMatrix(transformMatrix) {
            init();
        }

        Triangle::Triangle(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &normal1,
                           const Vector3 &normal2, const Vector3 &normal3, const Vector2 &uv1, const Vector2 &uv2,
                           const Vector2 &uv3, std::shared_ptr<Material> material,
                           std::shared_ptr<Matrix4> transformMatrix)
                : _position1(a), _position2(b), _position3(c),
                  _normal1(normal1), _normal2(normal2), _normal3(normal3),
                  _uv1(uv1), _uv2(uv2), _uv3(uv3),
                  _material(material), _transformMatrix(transformMatrix) {

            init();
        }

        void Triangle::init() {
            // 创建 AABB
            Vector3 transformedA = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_position1, 1.0f) : _position1;
            Vector3 transformedB = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_position2, 1.0f) : _position2;
            Vector3 transformedC = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_position3, 1.0f) : _position3;

            double minX = std::min(std::min(transformedA[0], transformedB[0]), transformedC[0]) - 0.0001;
            double minY = std::min(std::min(transformedA[1], transformedB[1]), transformedC[1]) - 0.0001;
            double minZ = std::min(std::min(transformedA[2], transformedB[2]), transformedC[2]) - 0.0001;

            double maxX = std::max(std::max(transformedA[0], transformedB[0]), transformedC[0]) + 0.0001;
            double maxY = std::max(std::max(transformedA[1], transformedB[1]), transformedC[1]) + 0.0001;
            double maxZ = std::max(std::max(transformedA[2], transformedB[2]), transformedC[2]) + 0.0001;

            _aabb = AABB(Vector3(minX, minY, minZ), Vector3(maxX, maxY, maxZ));
        }

        bool Triangle::hit(const Ray &ray, HitRecord &hitRecord,
                           double stepMin, double stepMax) {

            Vector3 transformedA = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_position1, 1.0f) : _position1;
            Vector3 transformedB = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_position2, 1.0f) : _position2;
            Vector3 transformedC = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_position3, 1.0f) : _position3;

            Vector3 transformedNormalA = (*_transformMatrix) * Vector4(_normal1, 0.0f);
            Vector3 transformedNormalB = (*_transformMatrix) * Vector4(_normal2, 0.0f);
            Vector3 transformedNormalC = (*_transformMatrix) * Vector4(_normal3, 0.0f);

            const Vector3 &dir = ray.getDirection();
            const Vector3 &eye = Vector3(ray.getOrigin().x, ray.getOrigin().y, ray.getOrigin().z);
            glm::mat3 equationParam(glm::vec3(transformedA - transformedB), glm::vec3(transformedA - transformedC),
                                    dir);

            if (abs(DETERMINANT(equationParam)) < EPSILON) {
                return false;
            }

            Vector3 equationResult = transformedA - eye;
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

                hitRecord.step = step;
                hitRecord.point = ray.at(hitRecord.step);
                Vector3 normal = alpha * transformedNormalA + ans[0] * transformedNormalB + ans[1] * transformedNormalC;
                hitRecord.setOutwardNormal(normal, dir);
                hitRecord.u = DOT(factor, Vector3(_uv1.x, _uv2.x, _uv3.x));
                hitRecord.v = DOT(factor, Vector3(_uv1.y, _uv2.y, _uv3.y));
                hitRecord.material = _material;
                hitRecord.id = getId();
                return true;
            } else {
                return false;
            }
        }

        const AABB &Triangle::boundingBox() const {
            return _aabb;
        }

    }
}