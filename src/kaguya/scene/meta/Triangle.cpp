//
// Created by Storm Phoenix on 2020/10/2.
//

#include <kaguya/scene/meta/Triangle.h>

namespace kaguya {
    namespace scene {

        Triangle::Triangle(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &normal1,
                           const Vector3 &normal2, const Vector3 &normal3, const Vector2 &uv1, const Vector2 &uv2,
                           const Vector2 &uv3, std::shared_ptr<Material> material,
                           std::shared_ptr<Matrix4> transformMatrix)
                : _a(a), _b(b), _c(c),
                  _normalA(normal1), _normalB(normal2), _normalC(normal3),
                  _uvA(uv1), _uvB(uv2), _uvC(uv3),
                  _material(material), _transformMatrix(transformMatrix) {

            // 创建 AABB
            Vector3 transformedA = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_a, 1.0f) : _a;
            Vector3 transformedB = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_b, 1.0f) : _b;
            Vector3 transformedC = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_c, 1.0f) : _c;

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
                                   (*_transformMatrix) * Vector4(_a, 1.0f) : _a;
            Vector3 transformedB = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_b, 1.0f) : _b;
            Vector3 transformedC = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_c, 1.0f) : _c;

            Vector3 transformedNormalA = (*_transformMatrix) * Vector4(_normalA, 0.0f);
            Vector3 transformedNormalB = (*_transformMatrix) * Vector4(_normalB, 0.0f);
            Vector3 transformedNormalC = (*_transformMatrix) * Vector4(_normalC, 0.0f);

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
            float alpha = 1 - ans[0] - ans[1];

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
                hitRecord.u = DOT(factor, Vector3(_uvA.x, _uvB.x, _uvC.x));
                hitRecord.v = DOT(factor, Vector3(_uvA.y, _uvB.y, _uvC.y));
                hitRecord.material = _material;
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