//
// Created by Storm Phoenix on 2020/10/7.
//

#include <kaguya/scene/meta/Wall.h>

namespace kaguya {
    namespace scene {
        namespace meta {
            Wall::Wall(Float width, Float height,
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

                Float minX = std::min(
                        std::min(std::min(_transformedLeftBottom[0], _transformedRightBottom[0]),
                                 _transformedRightTop[0]),
                        _transformedLeftTop[0]) - 0.0001;
                Float minY = std::min(
                        std::min(std::min(_transformedLeftBottom[1], _transformedRightBottom[1]),
                                 _transformedRightTop[1]),
                        _transformedLeftTop[1]) - 0.0001;
                Float minZ = std::min(
                        std::min(std::min(_transformedLeftBottom[2], _transformedRightBottom[2]),
                                 _transformedRightTop[2]),
                        _transformedLeftTop[2]) - 0.0001;

                Float maxX = std::max(
                        std::max(std::max(_transformedLeftBottom[0], _transformedRightBottom[0]),
                                 _transformedRightTop[0]),
                        _transformedLeftTop[0]) + 0.0001;
                Float maxY = std::max(
                        std::max(std::max(_transformedLeftBottom[1], _transformedRightBottom[1]),
                                 _transformedRightTop[1]),
                        _transformedLeftTop[1]) + 0.0001;
                Float maxZ = std::max(
                        std::max(std::max(_transformedLeftBottom[2], _transformedRightBottom[2]),
                                 _transformedRightTop[2]),
                        _transformedLeftTop[2]) + 0.0001;

                _aabb = AABB(Vector3F(minX, minY, minZ), Vector3F(maxX, maxY, maxZ));

                // 计算面积
                Float transformedWidth = LENGTH(_transformedRightBottom - _transformedLeftBottom);
                Float transformedHeight = LENGTH(_transformedLeftTop - _transformedLeftBottom);
                _area = (transformedWidth * transformedHeight);
            }

            bool Wall::intersect(Ray &ray, SurfaceInteraction &si,
                                 Float minStep, Float maxStep) const {
                // 仿照三角形平面求直线交点解法
                Vector3F transformedA = _transformMatrix->transformPoint(_leftTop);
                Vector3F transformedB = _transformMatrix->transformPoint(_leftBottom);
                Vector3F transformedC = _transformMatrix->transformPoint(_rightBottom);

                const Vector3F &dir = ray.getDirection();
                const Vector3F &eye = Vector3F(ray.getOrigin().x, ray.getOrigin().y, ray.getOrigin().z);
                glm::mat3 equationParam(glm::vec3(transformedA - transformedB), glm::vec3(transformedA - transformedC),
                                        dir);

                if (abs(DETERMINANT(equationParam)) < 10e-6f) {
                    return false;
                }

                Vector3F equationResult = transformedA - eye;
                Vector3F ans = INVERSE(equationParam) * equationResult;
                Float step = ans[2];
                Float alpha = 1 - ans[0] - ans[1];

                // 检查射线范围
                if (step < maxStep && step > minStep) {
                    Vector3F hitPoint = ray.at(step);
                    Vector3F modelPoint = _invTransformMatrix->transformPoint(hitPoint);

                    Float offsetX = modelPoint.x - (-_width / 2);
                    Float offsetY = modelPoint.y - (-_height / 2);

                    // 判断是否在四边形内部
                    if (math::checkRange(offsetX, 0, _width)
                        && math::checkRange(offsetY, 0, _height)) {
                        si.point = hitPoint;
                        Vector3F normal = _transformMatrix->transformNormal(_normal);
                        si.normal = si.rendering.normal = normal;
                        si.direction = dir;
                        si.u = (offsetX / _width);
                        si.v = (offsetY / _height);
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

            Float Wall::area() const {
                return _area;
            }

            SurfaceInteraction Wall::sampleSurfacePoint(Sampler *sampler1D) const {
                // 随机采样坐标
                Float u = sampler1D->sample1D();
                Float v = sampler1D->sample1D();

                Vector3F horizontal = (_transformedRightBottom - _transformedLeftBottom) * u;
                Vector3F vertical = (_transformedLeftTop - _transformedLeftBottom) * v;

                SurfaceInteraction si;
                si.point = (_transformedLeftBottom + horizontal + vertical);
                si.normal = si.rendering.normal = (_transformedNormal);
                return si;
            }

            Float Wall::surfacePointPdf(const SurfaceInteraction &si) const {
                Vector3F transformedPoint = _invTransformMatrix->transformPoint(si.point);

                if (transformedPoint.z - 0 <= math::EPSILON &&
                    transformedPoint.x >= -_width / 2 && transformedPoint.x <= _width / 2 &&
                    transformedPoint.y >= -_height / 2 && transformedPoint.y <= _height / 2) {
                    return 1.0 / area();
                } else {
                    return 0;
                }
            }

            ZXWall::ZXWall(Float z0, Float z1, Float x0, Float x1, Float y, bool upward) {
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
                                   Float minStep, Float maxStep) const {
                Float step = (_y - ray.getOrigin().y) / ray.getDirection().y;
                if (step > minStep && step < maxStep) {
                    Float z = ray.getOrigin().z + step * ray.getDirection().z;
                    Float x = ray.getOrigin().x + step * ray.getDirection().x;

                    if (math::checkRange(z, _z0, _z1) && math::checkRange(x, _x0, _x1)) {
                        si.point = (ray.at(step));
                        ray.setStep(step);
                        si.normal = si.rendering.normal = _normal;
                        si.direction = ray.getDirection();
                        si.u = ((x - _x0) / (_x1 - _x0));
                        si.v = ((z - _z0) / (_z1 - _z0));
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            }

            Float ZXWall::area() const {
                Float width = abs(_x1 - _x0);
                Float height = abs(_z1 - _z0);
                return width * height;
            }

            SurfaceInteraction ZXWall::sampleSurfacePoint(Sampler *sampler1D) const {
                Float u = sampler1D->sample1D();
                Float v = sampler1D->sample1D();

                Float width = abs(_x1 - _x0);
                Float height = abs(_z1 - _z0);

                SurfaceInteraction si;
                si.point = Vector3F({_x0 + width * u, _y, _z0 + height * v});
                si.normal = si.rendering.normal = (_normal);
                return si;
            }

            Float ZXWall::surfacePointPdf(const SurfaceInteraction &si) const {
                Vector3F samplePoint = si.point;
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

            YZWall::YZWall(Float y0, Float y1, Float z0, Float z1, Float x, bool rightward) {
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
                                   Float minStep, Float maxStep) const {
                Float step = (_x - ray.getOrigin().x) / ray.getDirection().x;
                if (step > minStep && step < maxStep) {
                    Float z = ray.getOrigin().z + step * ray.getDirection().z;
                    Float y = ray.getOrigin().y + step * ray.getDirection().y;

                    if (math::checkRange(z, _z0, _z1) && math::checkRange(y, _y0, _y1)) {
                        si.point = (ray.at(step));
                        ray.setStep(step);
                        si.normal = si.rendering.normal = _normal;
                        si.direction = ray.getDirection();
                        si.u = ((z - _z0) / (_z1 - _z0));
                        si.v = ((y - _y0) / (_y1 - _y0));
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

            Float YZWall::area() const {
                Float width = abs(_z1 - _z0);
                Float height = abs(_y1 - _y0);
                return width * height;
            }

            SurfaceInteraction YZWall::sampleSurfacePoint(Sampler *sampler1D) const {
                Float u = sampler1D->sample1D();
                Float v = sampler1D->sample1D();

                Float width = abs(_z1 - _z0);
                Float height = abs(_y1 - _y0);

                SurfaceInteraction si;
                si.point = Vector3F({_x, _y0 + v * height, _z0 + width * u});
                si.normal = si.rendering.normal = (_normal);
                return si;
            }

            Float YZWall::surfacePointPdf(const SurfaceInteraction &point) const {
                Vector3F samplePoint = point.point;
                if (samplePoint.y - _x < math::EPSILON &&
                    samplePoint.x >= _y0 && samplePoint.x <= _y1 &&
                    samplePoint.z >= _z0 && samplePoint.z <= _z1) {
                    return 1.0 / area();
                } else {
                    return 0;
                }
            }

            XYWall::XYWall(Float x0, Float x1, Float y0, Float y1, Float z, bool frontward) {
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
                                   Float minStep, Float maxStep) const {
                Float step = (_z - ray.getOrigin().z) / ray.getDirection().z;
                if (step > minStep && step < maxStep) {
                    Float x = ray.getOrigin().x + step * ray.getDirection().x;
                    Float y = ray.getOrigin().y + step * ray.getDirection().y;

                    if (math::checkRange(x, _x0, _x1) && math::checkRange(y, _y0, _y1)) {
                        si.point = (ray.at(step));
                        ray.setStep(step);
                        si.normal = si.rendering.normal = _normal;
                        si.direction = ray.getDirection();
                        si.u = ((x - _x0) / (_x1 - _x0));
                        si.v = ((y - _y0) / (_y1 - _y0));
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

            Float XYWall::area() const {
                Float width = abs(_x1 - _x0);
                Float height = abs(_y1 - _y0);
                return width * height;
            }

            SurfaceInteraction XYWall::sampleSurfacePoint(Sampler *sampler1D) const {
                Float u = sampler1D->sample1D();
                Float v = sampler1D->sample1D();

                Float width = abs(_x1 - _x0);
                Float height = abs(_y1 - _y0);

                SurfaceInteraction si;
                si.point = Vector3F({_x0 + width * u, _y0 + v * height, _z });
                si.normal = si.rendering.normal = (_normal);
                return si;
            }

            Float XYWall::surfacePointPdf(const SurfaceInteraction &si) const {
                Vector3F samplePoint = si.point;
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