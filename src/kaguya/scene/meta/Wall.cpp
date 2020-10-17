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

            init();
        }

        void Wall::init() {
            // build bounding box
            _transformedLeftBottom = _transformMatrix != nullptr ?
                                     (*_transformMatrix) * Vector4(_leftBottom, 1.0f) : _leftBottom;
            _transformedRightBottom = _transformMatrix != nullptr ?
                                      (*_transformMatrix) * Vector4(_rightBottom, 1.0f) : _rightBottom;
            _transformedRightTop = _transformMatrix != nullptr ?
                                   (*_transformMatrix) * Vector4(_rightTop, 1.0f) : _rightTop;
            _transformedLeftTop = _transformMatrix != nullptr ?
                                  (*_transformMatrix) * Vector4(_leftTop, 1.0f) : _leftTop;

            _transformedNormal = _transformMatrix != nullptr ?
                                 NORMALIZE(INVERSE_TRANSPOSE(*_transformMatrix) * Vector4(_normal, 0.0f)) :
                                 _normal;

            double minX = std::min(
                    std::min(std::min(_transformedLeftBottom[0], _transformedRightBottom[0]), _transformedRightTop[0]),
                    _transformedLeftTop[0]) - 0.0001;
            double minY = std::min(
                    std::min(std::min(_transformedLeftBottom[1], _transformedRightBottom[1]), _transformedRightTop[1]),
                    _transformedLeftTop[1]) - 0.0001;
            double minZ = std::min(
                    std::min(std::min(_transformedLeftBottom[2], _transformedRightBottom[2]), _transformedRightTop[2]),
                    _transformedLeftTop[2]) - 0.0001;

            double maxX = std::max(
                    std::max(std::max(_transformedLeftBottom[0], _transformedRightBottom[0]), _transformedRightTop[0]),
                    _transformedLeftTop[0]) + 0.0001;
            double maxY = std::max(
                    std::max(std::max(_transformedLeftBottom[1], _transformedRightBottom[1]), _transformedRightTop[1]),
                    _transformedLeftTop[1]) + 0.0001;
            double maxZ = std::max(
                    std::max(std::max(_transformedLeftBottom[2], _transformedRightBottom[2]), _transformedRightTop[2]),
                    _transformedLeftTop[2]) + 0.0001;

            _aabb = AABB(Vector3(minX, minY, minZ), Vector3(maxX, maxY, maxZ));

            // 计算面积
            double transformedWidth = LENGTH(_transformedRightBottom - _transformedLeftBottom);
            double transformedHeight = LENGTH(_transformedLeftTop - _transformedLeftBottom);
            _area = (transformedWidth * transformedHeight);
        }

        bool Wall::insect(const Ray &ray, Interaction &hitRecord,
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

        double Wall::area() {
            return _area;
        }

        Interaction Wall::sample() {
            // 随机采样坐标
            double u = uniformSample();
            double v = uniformSample();

            Vector3 horizontal = (_transformedRightBottom - _transformedLeftBottom) * u;
            Vector3 vertical = (_transformedLeftTop - _transformedLeftBottom) * v;

            Interaction samplePoint;
            samplePoint.point = _transformedLeftBottom + horizontal + vertical;
            samplePoint.normal = _transformedNormal;
            return samplePoint;
        }

        double Wall::pdf(Interaction &point) {
            Vector3 transformedPoint = _transformMatrix != nullptr ?
                                       INVERSE((*_transformMatrix)) * Vector4(point.point, 1.0f)
                                                                   : point.point;
            if (transformedPoint.z - 0 <= EPSILON &&
                transformedPoint.x > -_width / 2 && transformedPoint.x < _width / 2 &&
                transformedPoint.y > -_height / 2 && transformedPoint.y < _height / 2) {
                return 1.0 / area();
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
            init();
        }

        bool ZXWall::insect(const Ray &ray, Interaction &hitRecord,
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

        double ZXWall::area() {
            double width = abs(_x1 - _x0);
            double height = abs(_z1 - _z0);
            return width * height;
        }

        Interaction ZXWall::sample() {
            double u = uniformSample();
            double v = uniformSample();

            double width = abs(_x1 - _x0);
            double height = abs(_z1 - _z0);

            Interaction interaction;
            interaction.point = {_x0 + width * u, _y, _z0 + height * v};
            interaction.normal = _normal;
            return interaction;
        }

        double ZXWall::pdf(Interaction &point) {
            Vector3 samplePoint = point.point;
            if (samplePoint.y - _y < EPSILON &&
                samplePoint.x > _x0 && samplePoint.x < _x1 &&
                samplePoint.z > _z0 && samplePoint.z < _z1) {
                return 1.0 / area();
            } else {
                return 0;
            }
        }

        void ZXWall::init() {
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
            init();
        }

        bool YZWall::insect(const Ray &ray, Interaction &hitRecord,
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

        void YZWall::init() {
            _aabb = AABB({_x - 0.0001, _y0, _z0}, {_x + 0.0001, _y1, _z1});
        }

        double YZWall::area() {
            double width = abs(_z1 - _z0);
            double height = abs(_y1 - _y0);
            return width * height;
        }

        Interaction YZWall::sample() {
            double u = uniformSample();
            double v = uniformSample();

            double width = abs(_z1 - _z0);
            double height = abs(_y1 - _y0);

            Interaction interaction;
            interaction.point = {_x, _y0 + v * height, _z0 + width * u};
            interaction.normal = _normal;
            return interaction;
        }

        double YZWall::pdf(Interaction &point) {
            Vector3 samplePoint = point.point;
            if (samplePoint.y - _x < EPSILON &&
                samplePoint.x > _y0 && samplePoint.x < _y1 &&
                samplePoint.z > _z0 && samplePoint.z < _z1) {
                return 1.0 / area();
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
            init();
        }

        bool XYWall::insect(const Ray &ray, Interaction &hitRecord,
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

        void XYWall::init() {
            _aabb = AABB({_x0, _y0, _z - 0.0001}, {_x1, _y1, _z + 0.0001});
        }

        double XYWall::area() {
            double width = abs(_x1 - _x0);
            double height = abs(_y1 - _y0);
            return width * height;
        }

        Interaction XYWall::sample() {
            double u = uniformSample();
            double v = uniformSample();

            double width = abs(_x1 - _x0);
            double height = abs(_y1 - _y0);

            Interaction interaction;
            interaction.point = {_x0 + width * u, _y0 + v * height, _z};
            interaction.normal = _normal;
            return interaction;
        }

        double XYWall::pdf(Interaction &point) {
            Vector3 samplePoint = point.point;
            if (samplePoint.x - _z < EPSILON &&
                samplePoint.y > _y0 && samplePoint.x < _y1 &&
                samplePoint.x > _x0 && samplePoint.z < _x1) {
                return 1.0 / area();
            } else {
                return 0;
            }
        }

    }
}