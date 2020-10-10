//
// Created by Storm Phoenix on 2020/10/7.
//

#include <kaguya/scene/meta/Wall.h>

namespace kaguya {
    namespace scene {

        Wall::Wall(double width, double height, std::shared_ptr<Material> material,
                   std::shared_ptr<Matrix4> transformMatrix)
                : _width(width), _height(height),
                  _material(material),
                  _transformMatrix(transformMatrix) {
            _leftBottom = {-width / 2, -height / 2, 0.0f};
            _leftTop = {-width / 2, height / 2, 0.0f};
            _rightBottom = {width / 2, -height / 2, 0.0f};
            _rightTop = {width / 2, height / 2, 0.0f};
            _normal = {0.0f, 0.0f, 1.0f};

            buildBoundingBox();
        }

        void Wall::buildBoundingBox() {
            Vector3 transformedA = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_leftBottom, 1.0f) : _leftBottom;
            Vector3 transformedB = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_rightBottom, 1.0f) : _rightBottom;
            Vector3 transformedC = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_rightTop, 1.0f) : _rightTop;
            Vector3 transformedD = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_leftTop, 1.0f) : _leftTop;

            double minX = std::min(std::min(std::min(transformedA[0], transformedB[0]), transformedC[0]),
                                   transformedD[0]) - 0.0001;
            double minY = std::min(std::min(std::min(transformedA[1], transformedB[1]), transformedC[1]),
                                   transformedD[1]) - 0.0001;
            double minZ = std::min(std::min(std::min(transformedA[2], transformedB[2]), transformedC[2]),
                                   transformedD[2]) - 0.0001;

            double maxX = std::max(std::max(std::max(transformedA[0], transformedB[0]), transformedC[0]),
                                   transformedD[0]) + 0.0001;
            double maxY = std::max(std::max(std::max(transformedA[1], transformedB[1]), transformedC[1]),
                                   transformedD[1]) + 0.0001;
            double maxZ = std::max(std::max(std::max(transformedA[2], transformedB[2]), transformedC[2]),
                                   transformedD[2]) + 0.0001;

            _aabb = AABB(Vector3(minX, minY, minZ), Vector3(maxX, maxY, maxZ));
        }

        bool Wall::hit(const Ray &ray, HitRecord &hitRecord,
                       double stepMin, double stepMax) {
            // 仿照三角形平面求直线交点解法
            Vector3 transformedA = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_leftTop, 1.0f) : _leftTop;
            Vector3 transformedB = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_leftBottom, 1.0f) : _leftBottom;
            Vector3 transformedC = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_rightBottom, 1.0f) : _rightBottom;

            const Vector3 &dir = ray.getDirection();
            const Vector3 &eye = Vector3(ray.getOrigin().x, ray.getOrigin().y, ray.getOrigin().z);
            glm::mat3 equationParam(glm::vec3(transformedA - transformedB), glm::vec3(transformedA - transformedC),
                                    dir);

            if (abs(DETERMINANT(equationParam)) < 10e-6f) {
                return false;
            }

            Vector3 equationResult = transformedA - eye;
            Vector3 ans = INVERSE(equationParam) * equationResult;
            double step = ans[2];
            double alpha = 1 - ans[0] - ans[1];

            // 检查射线范围
            if (step < stepMax && step > stepMin) {
                Vector3 hitPoint = ray.at(step);
                Vector3 modelPoint = INVERSE(*_transformMatrix) * Vector4(hitPoint, 1.0f);

                double offsetX = modelPoint.x - (-_width / 2);
                double offsetY = modelPoint.y - (-_height / 2);

                // 判断是否在四边形内部
                if (checkRange(offsetX, 0, _width)
                    && checkRange(offsetY, 0, _height)) {
                    hitRecord.step = step;
                    hitRecord.point = hitPoint;
                    Vector3 normal = (*_transformMatrix) * Vector4(_normal, 0.0f);
                    hitRecord.setOutwardNormal(normal, dir);
                    hitRecord.u = offsetX / _width;
                    hitRecord.v = offsetY / _height;
                    hitRecord.material = _material;
                    hitRecord.id = getId();
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }

        const AABB &Wall::boundingBox() const {
            return _aabb;
        }

        Vector3 Wall::samplePoint(double &pdf, Vector3 &normal) {
            // 随机采样坐标
            double u = uniformSample();
            double v = uniformSample();

            Vector3 transformedLeftBottom = _transformMatrix != nullptr ?
                                            (*_transformMatrix) * Vector4(_leftBottom, 1.0f) : _leftBottom;

            Vector3 transformedRightBottom = _transformMatrix != nullptr ?
                                             (*_transformMatrix) * Vector4(_rightBottom, 1.0f) : _rightBottom;

            Vector3 transformedLeftTop = _transformMatrix != nullptr ?
                                         (*_transformMatrix) * Vector4(_leftTop, 1.0f) : _leftTop;

            Vector3 horizontal = (transformedRightBottom - transformedLeftBottom) * u;
            Vector3 vertical = (transformedLeftTop - transformedLeftBottom) * v;

            pdf = 1.0 / (LENGTH(transformedRightBottom - transformedLeftBottom) *
                         LENGTH(transformedLeftTop - transformedLeftBottom));
            normal = NORMALIZE((*_transformMatrix) * Vector4(_normal, 0.0f));
            return transformedLeftBottom + horizontal + vertical;
        }

        double Wall::samplePointPdf(Vector3 &point) {
            Vector3 transformedPoint =
                    _transformMatrix != nullptr ? INVERSE((*_transformMatrix)) * Vector4(point, 1.0f) : point;
            if (transformedPoint.z - 0 <= EPSILON &&
                transformedPoint.x > -_width / 2 && transformedPoint.x < _width / 2 &&
                transformedPoint.y > -_height / 2 && transformedPoint.y < _height / 2) {
                Vector3 transformedLeftBottom = (*_transformMatrix) * Vector4(_leftBottom, 1.0);
                Vector3 transformedRightBottom = (*_transformMatrix) * Vector4(_rightBottom, 1.0);
                Vector3 transformedLeftTop = (*_transformMatrix) * Vector4(_leftTop, 1.0);

                double transformedWidth = LENGTH(transformedRightBottom - transformedLeftBottom);
                double transformedHeight = LENGTH(transformedLeftTop - transformedLeftBottom);

                return 1.0 / (transformedWidth * transformedHeight);
            } else {
                return 0;
            }
        }

        ZXWall::ZXWall(double z0, double z1, double x0, double x1, double y, bool upward,
                       std::shared_ptr<Material> material) {
            _y = y;
            _z0 = z0;
            _z1 = z1;
            _x0 = x0;
            _x1 = x1;
            _material = material;

            if (upward) {
                _normal = {0, 1, 0};
            } else {
                _normal = {0, -1, 0};
            }
            buildBoundingBox();
        }

        bool ZXWall::hit(const Ray &ray, HitRecord &hitRecord,
                         double stepMin, double stepMax) {
            double step = (_y - ray.getOrigin().y) / ray.getDirection().y;
            if (step >= stepMin && step <= stepMax) {
                double z = ray.getOrigin().z + step * ray.getDirection().z;
                double x = ray.getOrigin().x + step * ray.getDirection().x;

                if (checkRange(z, _z0, _z1) && checkRange(x, _x0, _x1)) {
                    hitRecord.material = _material;
                    hitRecord.id = getId();
                    hitRecord.point = ray.at(step);
                    hitRecord.step = step;
                    hitRecord.setOutwardNormal(_normal, ray.getDirection());
                    hitRecord.u = (x - _x0) / (_x1 - _x0);
                    hitRecord.v = (z - _z0) / (_z1 - _z0);
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }

        Vector3 ZXWall::samplePoint(double &pdf, Vector3 &normal) {
            double u = uniformSample();
            double v = uniformSample();

            double width = abs(_x1 - _x0);
            double height = abs(_z1 - _z0);

            pdf = 1 / (width * height);
            normal = _normal;
            return {_x0 + width * u, _y, _z0 + height * v};
        }

        double ZXWall::samplePointPdf(Vector3 &point) {
            if (point.y - _y < EPSILON &&
                point.x > _x0 && point.x < _x1 &&
                point.z > _z0 && point.z < _z1) {
                return 1.0 / (abs(_z0 - _z1) * abs(_x0 - _x1));
            } else {
                return 0;
            }
        }

        void ZXWall::buildBoundingBox() {
            _aabb = AABB({_x0, _y - 0.0001, _z0}, {_x1, _y + 0.0001, _z1});
        }

        YZWall::YZWall(double y0, double y1, double z0, double z1, double x, bool rightward,
                       std::shared_ptr<Material> material) {
            _x = x;
            _y0 = y0;
            _y1 = y1;
            _z0 = z0;
            _z1 = z1;
            _material = material;

            if (rightward) {
                _normal = {1, 0, 0};
            } else {
                _normal = {-1, 0, 0};
            }
            buildBoundingBox();
        }

        bool YZWall::hit(const Ray &ray, HitRecord &hitRecord,
                         double stepMin, double stepMax) {
            double step = (_x - ray.getOrigin().x) / ray.getDirection().x;
            if (step >= stepMin && step <= stepMax) {
                double z = ray.getOrigin().z + step * ray.getDirection().z;
                double y = ray.getOrigin().y + step * ray.getDirection().y;

                if (checkRange(z, _z0, _z1) && checkRange(y, _y0, _y1)) {
                    hitRecord.material = _material;
                    hitRecord.id = getId();
                    hitRecord.point = ray.at(step);
                    hitRecord.step = step;
                    hitRecord.setOutwardNormal(_normal, ray.getDirection());
                    hitRecord.u = (z - _z0) / (_z1 - _z0);
                    hitRecord.v = (y - _y0) / (_y1 - _y0);
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }

        void YZWall::buildBoundingBox() {
            _aabb = AABB({_x - 0.0001, _y0, _z0}, {_x + 0.0001, _y1, _z1});
        }

        Vector3 YZWall::samplePoint(double &pdf, Vector3 &normal) {
            double u = uniformSample();
            double v = uniformSample();

            double width = abs(_z1 - _z0);
            double height = abs(_y1 - _y0);

            pdf = 1 / (width * height);
            normal = _normal;
            return {_x, _y0 + v * height, _z0 + width * u};
        }

        double YZWall::samplePointPdf(Vector3 &point) {
            if (point.x - _x < EPSILON &&
                point.y > _y0 && point.x < _y1 &&
                point.z > _z0 && point.z < _z1) {
                return 1.0 / (abs(_z0 - _z1) * abs(_y0 - _y1));
            } else {
                return 0;
            }
        }

        XYWall::XYWall(double x0, double x1, double y0, double y1, double z, bool frontward,
                       std::shared_ptr<Material> material) {
            _z = z;
            _x0 = x0;
            _x1 = x1;
            _y0 = y0;
            _y1 = y1;
            _material = material;

            if (frontward) {
                _normal = {0, 0, -1};
            } else {
                _normal = {0, 0, 1};
            }
            buildBoundingBox();
        }

        bool XYWall::hit(const Ray &ray, HitRecord &hitRecord,
                         double stepMin, double stepMax) {
            double step = (_z - ray.getOrigin().z) / ray.getDirection().z;
            if (step >= stepMin && step <= stepMax) {
                double x = ray.getOrigin().x + step * ray.getDirection().x;
                double y = ray.getOrigin().y + step * ray.getDirection().y;

                if (checkRange(x, _x0, _x1) && checkRange(y, _y0, _y1)) {
                    hitRecord.material = _material;
                    hitRecord.id = getId();
                    hitRecord.point = ray.at(step);
                    hitRecord.step = step;
                    hitRecord.setOutwardNormal(_normal, ray.getDirection());
                    hitRecord.u = (x - _x0) / (_x1 - _x0);
                    hitRecord.v = (y - _y0) / (_y1 - _y0);
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }

        void XYWall::buildBoundingBox() {
            _aabb = AABB({_x0, _y0, _z - 0.0001}, {_x1, _y1, _z + 0.0001});
        }

        Vector3 XYWall::samplePoint(double &pdf, Vector3 &normal) {
            double u = uniformSample();
            double v = uniformSample();

            double width = abs(_x1 - _x0);
            double height = abs(_y1 - _y0);

            pdf = 1 / (width * height);
            normal = _normal;
            return {_x0 + width * u, _y0 + v * height, _z};
        }

        double XYWall::samplePointPdf(Vector3 &point) {
            if (point.x - _z < EPSILON &&
                point.y > _y0 && point.x < _y1 &&
                point.x > _x0 && point.z < _x1) {
                return 1.0 / (abs(_x0 - _x1) * abs(_y0 - _y1));
            } else {
                return 0;
            }
        }
    }
}