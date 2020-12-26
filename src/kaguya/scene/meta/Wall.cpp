//
// Created by Storm Phoenix on 2020/10/7.
//

#include <kaguya/scene/meta/Wall.h>

namespace kaguya {
    namespace scene {
        namespace meta {
            Wall::Wall(double width, double height,
                       std::shared_ptr<Transform> transformMatrix)
                    : _width(width), _height(height),
                      _transformMatrix(transformMatrix),
                      _invTransformMatrix(transformMatrix->inverse()) {
                _leftBottom = {-width / 2, -height / 2, 0.0f};
                _leftTop = {-width / 2, height / 2, 0.0f};
                _rightBottom = {width / 2, -height / 2, 0.0f};
                _rightTop = {width / 2, height / 2, 0.0f};
                _normal = {0.0f, 0.0f, 1.0f};

                init();
            }

            void Wall::init() {
                // build bounding box
                _transformedLeftBottom = _transformMatrix->transformPoint(_leftBottom);
                _transformedRightBottom = _transformMatrix->transformPoint(_rightBottom);
                _transformedRightTop = _transformMatrix->transformPoint(_rightTop);
                _transformedLeftTop = _transformMatrix->transformPoint(_leftTop);
                _transformedNormal = _transformMatrix->transformNormal(_normal);

                double minX = std::min(
                        std::min(std::min(_transformedLeftBottom[0], _transformedRightBottom[0]),
                                 _transformedRightTop[0]),
                        _transformedLeftTop[0]) - 0.0001;
                double minY = std::min(
                        std::min(std::min(_transformedLeftBottom[1], _transformedRightBottom[1]),
                                 _transformedRightTop[1]),
                        _transformedLeftTop[1]) - 0.0001;
                double minZ = std::min(
                        std::min(std::min(_transformedLeftBottom[2], _transformedRightBottom[2]),
                                 _transformedRightTop[2]),
                        _transformedLeftTop[2]) - 0.0001;

                double maxX = std::max(
                        std::max(std::max(_transformedLeftBottom[0], _transformedRightBottom[0]),
                                 _transformedRightTop[0]),
                        _transformedLeftTop[0]) + 0.0001;
                double maxY = std::max(
                        std::max(std::max(_transformedLeftBottom[1], _transformedRightBottom[1]),
                                 _transformedRightTop[1]),
                        _transformedLeftTop[1]) + 0.0001;
                double maxZ = std::max(
                        std::max(std::max(_transformedLeftBottom[2], _transformedRightBottom[2]),
                                 _transformedRightTop[2]),
                        _transformedLeftTop[2]) + 0.0001;

                _aabb = AABB(Vector3(minX, minY, minZ), Vector3(maxX, maxY, maxZ));

                // 计算面积
                double transformedWidth = LENGTH(_transformedRightBottom - _transformedLeftBottom);
                double transformedHeight = LENGTH(_transformedLeftTop - _transformedLeftBottom);
                _area = (transformedWidth * transformedHeight);
            }

            bool Wall::intersect(Ray &ray, SurfaceInteraction &si,
                                 double minStep, double maxStep) const {
                // 仿照三角形平面求直线交点解法
                Vector3 transformedA = _transformMatrix->transformPoint(_leftTop);
                Vector3 transformedB = _transformMatrix->transformPoint(_leftBottom);
                Vector3 transformedC = _transformMatrix->transformPoint(_rightBottom);

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
                if (step < maxStep && step > minStep) {
                    Vector3 hitPoint = ray.at(step);
                    Vector3 modelPoint = _invTransformMatrix->transformPoint(hitPoint);

                    double offsetX = modelPoint.x - (-_width / 2);
                    double offsetY = modelPoint.y - (-_height / 2);

                    // 判断是否在四边形内部
                    if (math::checkRange(offsetX, 0, _width)
                        && math::checkRange(offsetY, 0, _height)) {
                        si.setPoint(hitPoint);
                        Vector3 normal = _transformMatrix->transformNormal(_normal);
                        si.setOutwardNormal(normal, dir);
                        si.setU(offsetX / _width);
                        si.setV(offsetY / _height);
                        ray.setStep(step);
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            }

            const AABB &Wall::bound() const {
                return _aabb;
            }

            double Wall::area() const {
                return _area;
            }

            SurfaceInteraction Wall::sampleSurfacePoint(const Sampler *sampler1D) const {
                // 随机采样坐标
                double u = sampler1D->sample1d();
                double v = sampler1D->sample1d();

                Vector3 horizontal = (_transformedRightBottom - _transformedLeftBottom) * u;
                Vector3 vertical = (_transformedLeftTop - _transformedLeftBottom) * v;

                SurfaceInteraction si;
                si.setPoint(_transformedLeftBottom + horizontal + vertical);
                si.setNormal(_transformedNormal);
                return si;
            }

            double Wall::surfacePointPdf(const SurfaceInteraction &si) const {
                Vector3 transformedPoint = _invTransformMatrix->transformPoint(si.getPoint());

                if (transformedPoint.z - 0 <= math::EPSILON &&
                    transformedPoint.x >= -_width / 2 && transformedPoint.x <= _width / 2 &&
                    transformedPoint.y >= -_height / 2 && transformedPoint.y <= _height / 2) {
                    return 1.0 / area();
                } else {
                    return 0;
                }
            }

            ZXWall::ZXWall(double z0, double z1, double x0, double x1, double y, bool upward) {
                _y = y;
                _z0 = z0;
                _z1 = z1;
                _x0 = x0;
                _x1 = x1;

                if (upward) {
                    _normal = {0, 1, 0};
                } else {
                    _normal = {0, -1, 0};
                }
                init();
            }

            bool ZXWall::intersect(Ray &ray, SurfaceInteraction &si,
                                   double minStep, double maxStep) const {
                double step = (_y - ray.getOrigin().y) / ray.getDirection().y;
                if (step > minStep && step < maxStep) {
                    double z = ray.getOrigin().z + step * ray.getDirection().z;
                    double x = ray.getOrigin().x + step * ray.getDirection().x;

                    if (math::checkRange(z, _z0, _z1) && math::checkRange(x, _x0, _x1)) {
                        si.setPoint(ray.at(step));
                        ray.setStep(step);
                        si.setOutwardNormal(_normal, ray.getDirection());
                        si.setU((x - _x0) / (_x1 - _x0));
                        si.setV((z - _z0) / (_z1 - _z0));
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            }

            double ZXWall::area() const {
                double width = abs(_x1 - _x0);
                double height = abs(_z1 - _z0);
                return width * height;
            }

            SurfaceInteraction ZXWall::sampleSurfacePoint(const Sampler *sampler1D) const {
                double u = sampler1D->sample1d();
                double v = sampler1D->sample1d();

                double width = abs(_x1 - _x0);
                double height = abs(_z1 - _z0);

                SurfaceInteraction interaction;
                interaction.setPoint({_x0 + width * u, _y, _z0 + height * v});
                interaction.setNormal(_normal);
                return interaction;
            }

            double ZXWall::surfacePointPdf(const SurfaceInteraction &si) const {
                Vector3 samplePoint = si.getPoint();
                if (samplePoint.y - _y < math::EPSILON &&
                    samplePoint.x >= _x0 && samplePoint.x <= _x1 &&
                    samplePoint.z >= _z0 && samplePoint.z <= _z1) {
                    return 1.0 / area();
                } else {
                    return 0;
                }
            }

            void ZXWall::init() {
                _aabb = AABB({_x0, _y - 0.0001, _z0}, {_x1, _y + 0.0001, _z1});
            }

            YZWall::YZWall(double y0, double y1, double z0, double z1, double x, bool rightward) {
                _x = x;
                _y0 = y0;
                _y1 = y1;
                _z0 = z0;
                _z1 = z1;

                if (rightward) {
                    _normal = {1, 0, 0};
                } else {
                    _normal = {-1, 0, 0};
                }
                init();
            }

            bool YZWall::intersect(Ray &ray, SurfaceInteraction &si,
                                   double minStep, double maxStep) const {
                double step = (_x - ray.getOrigin().x) / ray.getDirection().x;
                if (step > minStep && step < maxStep) {
                    double z = ray.getOrigin().z + step * ray.getDirection().z;
                    double y = ray.getOrigin().y + step * ray.getDirection().y;

                    if (math::checkRange(z, _z0, _z1) && math::checkRange(y, _y0, _y1)) {
                        si.setPoint(ray.at(step));
                        ray.setStep(step);
                        si.setOutwardNormal(_normal, ray.getDirection());
                        si.setU((z - _z0) / (_z1 - _z0));
                        si.setV((y - _y0) / (_y1 - _y0));
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

            double YZWall::area() const {
                double width = abs(_z1 - _z0);
                double height = abs(_y1 - _y0);
                return width * height;
            }

            SurfaceInteraction YZWall::sampleSurfacePoint(const Sampler *sampler1D) const {
                double u = sampler1D->sample1d();
                double v = sampler1D->sample1d();

                double width = abs(_z1 - _z0);
                double height = abs(_y1 - _y0);

                SurfaceInteraction interaction;
                interaction.setPoint({_x, _y0 + v * height, _z0 + width * u});
                interaction.setNormal(_normal);
                return interaction;
            }

            double YZWall::surfacePointPdf(const SurfaceInteraction &point) const {
                Vector3 samplePoint = point.getPoint();
                if (samplePoint.y - _x < math::EPSILON &&
                    samplePoint.x >= _y0 && samplePoint.x <= _y1 &&
                    samplePoint.z >= _z0 && samplePoint.z <= _z1) {
                    return 1.0 / area();
                } else {
                    return 0;
                }
            }

            XYWall::XYWall(double x0, double x1, double y0, double y1, double z, bool frontward) {
                _z = z;
                _x0 = x0;
                _x1 = x1;
                _y0 = y0;
                _y1 = y1;

                if (frontward) {
                    _normal = {0, 0, -1};
                } else {
                    _normal = {0, 0, 1};
                }
                init();
            }

            bool XYWall::intersect(Ray &ray, SurfaceInteraction &si,
                                   double minStep, double maxStep) const {
                double step = (_z - ray.getOrigin().z) / ray.getDirection().z;
                if (step > minStep && step < maxStep) {
                    double x = ray.getOrigin().x + step * ray.getDirection().x;
                    double y = ray.getOrigin().y + step * ray.getDirection().y;

                    if (math::checkRange(x, _x0, _x1) && math::checkRange(y, _y0, _y1)) {
                        si.setPoint(ray.at(step));
                        ray.setStep(step);
                        si.setOutwardNormal(_normal, ray.getDirection());
                        si.setU((x - _x0) / (_x1 - _x0));
                        si.setV((y - _y0) / (_y1 - _y0));
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

            double XYWall::area() const {
                double width = abs(_x1 - _x0);
                double height = abs(_y1 - _y0);
                return width * height;
            }

            SurfaceInteraction XYWall::sampleSurfacePoint(const Sampler *sampler1D) const {
                double u = sampler1D->sample1d();
                double v = sampler1D->sample1d();

                double width = abs(_x1 - _x0);
                double height = abs(_y1 - _y0);

                SurfaceInteraction interaction;
                interaction.setPoint({_x0 + width * u, _y0 + v * height, _z});
                interaction.setNormal(_normal);
                return interaction;
            }

            double XYWall::surfacePointPdf(const SurfaceInteraction &si) const {
                Vector3 samplePoint = si.getPoint();
                if (samplePoint.x - _z < math::EPSILON &&
                    samplePoint.y >= _y0 && samplePoint.x <= _y1 &&
                    samplePoint.x >= _x0 && samplePoint.z <= _x1) {
                    return 1.0 / area();
                } else {
                    return 0;
                }
            }
        }
    }
}