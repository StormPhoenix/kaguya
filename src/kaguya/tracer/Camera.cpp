//
// Created by Storm Phoenix on 2020/9/30.
//

#include <kaguya/tracer/Camera.h>

namespace kaguya {
    namespace tracer {

        Camera::Camera(const Vector3 &eye, const Vector3 &direction, float fov, float aspect) {
            _eye = eye;
            _front = NORMALIZE(direction);
            buildCameraCoordinate(fov, aspect);
        }

        Camera::Camera(const Vector3 &eye, float yaw, float pitch, float fov, float aspect) {
            _eye = eye;
            _front.x = cos(DEGREES_TO_RADIANS(pitch)) * cos(DEGREES_TO_RADIANS(yaw));
            _front.y = sin(DEGREES_TO_RADIANS(pitch));
            _front.z = cos(DEGREES_TO_RADIANS(pitch)) * sin(DEGREES_TO_RADIANS(yaw));
            _front = NORMALIZE(_front);
            buildCameraCoordinate(fov, aspect);
        }

        Ray Camera::sendRay(double u, double v) {
            Vector3 samplePoint =
                    _leftBottomCorner + 2 * _halfWindowWidth * u * _right + 2 * _halfWindowHeight * v * _up;
            Vector3 dir = NORMALIZE(samplePoint - _eye);
            return Ray(_eye, dir);
        }

        Point2d Camera::getFilmPosition(const Vector3 &dir) {
            // dir 在 _front 上的投影长度
            double frontLen = ABS_DOT(dir, _front);
            double factor = _focal / frontLen;
            // 射线投射到成像平面，并计算成像平面左下角到投射点的向量
            Vector3 posVector = _eye + dir * factor - _leftBottomCorner;
            double u = DOT(posVector, _right) / (2 * _halfWindowWidth);
            double v = DOT(posVector, _up) / (2 * _halfWindowHeight);
            return Point2d(u * _resolutionWidth, v * _resolutionHeight);
        }

        Vector3 Camera::getEye() const {
            return _eye;
        }

        int Camera::getResolutionHeight() {
            return _resolutionHeight;
        }

        int Camera::getResolutionWidth() {
            return _resolutionWidth;
        }

        void Camera::setResolutionHeight(int resolutionHeight) {
            _resolutionHeight = resolutionHeight;
        }

        void Camera::setResolutionWidth(int resolutionWidth) {
            _resolutionWidth = resolutionWidth;
        }

        FilmPlane *Camera::buildFilmPlane(int channel) {
            return new FilmPlane(_resolutionWidth, _resolutionHeight, channel);
        }

        Spectrum Camera::sampleCameraRay(const Interaction &eye,
                                         Vector3 *wi, double *pdf,
                                         Point2d *filmPosition,
                                         VisibilityTester *visibilityTester) const {
            // 在相机镜头圆盘上随机采样
            Vector2 diskSample = diskUniformSampling(_lensRadius);
            // 计算相机镜头采样点 3D 坐标
            Vector3 lensSample = _right * diskSample.x + _up * diskSample.y + _eye;

            // 计算射线方向
            (*wi) = lensSample - eye.getPoint();
            double dist = LENGTH(*wi);
            (*wi) = NORMALIZE(*wi);

            // 创建 Interaction
            Interaction lensInter = Interaction(lensSample, *wi, _front, dist);
            // 设置 visibility tester
            (*visibilityTester) = VisibilityTester(eye, lensInter);

            // lensInter 向 eye 发射射线的 pdf
            double lensArea = PI * _lensRadius * _lensRadius;
            (*pdf) = (dist * dist) / (lensArea * ABS_DOT(lensInter.getNormal(), *wi));

            return importance(Ray(lensSample, -(*wi)), filmPosition);
        }

        Spectrum Camera::importance(const Ray &ray, Point2d *filmPosition) const {
            // 计算新射线光栅化位置
            double step = _focal / ABS_DOT(ray.getDirection(), _front);
            Vector3 raster = ray.getOrigin() + ray.getDirection() * step - _leftBottomCorner;
            double u = DOT(raster, _right) / (DOT(_diagonalVector, _right));
            double v = DOT(raster, _up) / (DOT(_diagonalVector, _up));
            (*filmPosition) = Point2d(u * _resolutionWidth, v * _resolutionHeight);

            // 如果超出边界，则 importance 为 0
            if (std::floor(filmPosition->x) < 0 || std::floor(filmPosition->x) >= _resolutionWidth ||
                std::floor(filmPosition->y) < 0 || std::floor(filmPosition->y) >= _resolutionHeight) {
                return Spectrum(0.);
            }

            /*
             * 计算相机采样的重要性，相机采样的重要性是基于:
             * 在镜头 lens 和成像平面 film plane 上同时采样点的重要性
             *
             * 推导:
             * 1) p(w) = dist^2 / (A * cosine)
             * 2) dist * cosine = _focal
             * 3) W_e = p(w) / (PI * lensRadius^2 * cosine)
             * 4) A is area of image plane
             * 5) W_e = _focal^2 / (cosine^4 * A * PI * lensRadius^2)
             **/

            double cosine = ABS_DOT(ray.getDirection(), _front);
            /*
            double dist = _focal / cosine;
            double pW = dist * dist / (_area * cosine);
            double weight = pW / (PI * _lensRadius * _lensRadius * cosine);
             */
            double weight = (_focal * _focal) /
                            (_area * PI * _lensRadius * _lensRadius * std::pow(cosine, 4));
            return Spectrum(weight);
        }

        void Camera::buildCameraCoordinate(float fov, float aspect) {
            Vector3 worldUp = Vector3(0.0f, 1.0f, 0.0f);
            // 判断 _front 方向是否与 worldUp 重叠
            if (abs(_front.x) < EPSILON && abs(_front.z) < EPSILON) {
                _right = {1.0f, 0.0f, 0.0f};
            } else {
                _right = NORMALIZE(CROSS(_front, worldUp));
            }
            _up = NORMALIZE(CROSS(_right, _front));

            _halfWindowHeight = tan(DEGREES_TO_RADIANS(fov / 2)) * _focal;
            _halfWindowWidth = _halfWindowHeight * aspect;
            _area = _halfWindowHeight * _halfWindowWidth;
            _leftBottomCorner = _eye + _focal * _front - _halfWindowWidth * _right - _halfWindowHeight * _up;
            _rightTopCorner = _eye + _focal * _front + _halfWindowWidth * _right + _halfWindowHeight * _up;
            _diagonalVector = _rightTopCorner - _leftBottomCorner;
        }
    }
}