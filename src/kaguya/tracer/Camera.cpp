//
// Created by Storm Phoenix on 2020/9/30.
//

#include <kaguya/tracer/Camera.h>
#include <kaguya/core/medium/MediumBound.h>

namespace kaguya {
    namespace tracer {

        using kaguya::core::medium::MediumBound;

        Camera::Camera(const Vector3d &eye, const Vector3d &direction, const std::shared_ptr<Medium> medium, float fov,
                       float aspect) : _medium(medium) {
            _eye = eye;
            _front = NORMALIZE(direction);
            buildCameraCoordinate(fov, aspect);
        }

        Camera::Camera(const Vector3d &eye, float yaw, float pitch,
                       const std::shared_ptr<Medium> medium, float fov, float aspect) : _medium(medium) {
            _eye = eye;
            _front.x = cos(math::DEGREES_TO_RADIANS(pitch)) * cos(math::DEGREES_TO_RADIANS(yaw));
            _front.y = sin(math::DEGREES_TO_RADIANS(pitch));
            _front.z = cos(math::DEGREES_TO_RADIANS(pitch)) * sin(math::DEGREES_TO_RADIANS(yaw));
            _front = NORMALIZE(_front);
            buildCameraCoordinate(fov, aspect);
        }

        Ray Camera::sendRay(double u, double v) const {
            Vector3d samplePoint =
                    _leftBottomCorner + 2 * _halfWindowWidth * u * _right + 2 * _halfWindowHeight * v * _up;
            Vector3d dir = NORMALIZE(samplePoint - _eye);
            return Ray(_eye, dir, _medium.get());
        }

        Vector3d Camera::getEye() const {
            return _eye;
        }

        int Camera::getResolutionHeight() const {
            return _resolutionHeight;
        }

        int Camera::getResolutionWidth() const {
            return _resolutionWidth;
        }

        void Camera::setResolutionHeight(int resolutionHeight) {
            _resolutionHeight = resolutionHeight;
        }

        void Camera::setResolutionWidth(int resolutionWidth) {
            _resolutionWidth = resolutionWidth;
        }

        FilmPlane *Camera::buildFilmPlane(int channel) const {
            return new FilmPlane(_resolutionWidth, _resolutionHeight, channel);
        }

        Spectrum Camera::sampleCameraRay(const Interaction &eye,
                                         Vector3d *wi, double *pdf,
                                         Point2d *filmPosition,
                                         Sampler *const sampler,
                                         VisibilityTester *visibilityTester) const {
            // 在相机镜头圆盘上随机采样
            Vector2d diskSample = math::diskUniformSampling(sampler, _lensRadius);
            // 计算相机镜头采样点 3D 坐标
            Vector3d lensSample = _right * diskSample.x + _up * diskSample.y + _eye;

            // 计算射线方向
            (*wi) = lensSample - eye.getPoint();
            double dist = LENGTH(*wi);
            (*wi) = NORMALIZE(*wi);

            // 创建 Interaction
            Interaction lensInter = Interaction(lensSample, *wi, _front, _medium.get());
            // 设置 visibility tester
            (*visibilityTester) = VisibilityTester(eye, lensInter);

            // lensInter 向 eye 发射射线的 pdf
            double lensArea = math::PI * _lensRadius * _lensRadius;
            // 从 eye 位置对相机成像平面采样 pdf
            (*pdf) = (dist * dist) / (lensArea * ABS_DOT(lensInter.getNormal(), *wi));

            return rayImportance(Ray(lensSample, -(*wi)), filmPosition);
        }

        void Camera::rayImportance(const Ray &ray, double &pdfPos, double &pdfDir) const {
            // 判断射线是否从正面发射出去
            double cosine = DOT(ray.getDirection(), _front);
            if (cosine <= 0) {
                pdfPos = 0.0;
                pdfDir = 0.0;
                return;
            }

            // 判断射线是否击中成像平面
            double step = _focal / cosine;
            Vector3d raster = ray.getOrigin() + ray.getDirection() * step - _leftBottomCorner;
            double u = DOT(raster, _right) / (DOT(_diagonalVector, _right));
            double v = DOT(raster, _up) / (DOT(_diagonalVector, _up));
            if (u < 0 || u >= 1 || v < 0 || v >= 1) {
                pdfPos = 0.0;
                pdfDir = 0.0;
                return;
            }

            // 计算 pdfPos pdfDir
            pdfPos = 1 / (math::PI * _lensRadius * _lensRadius);
            pdfDir = (_focal * _focal) / (_area * cosine * cosine * cosine);
        }

        Spectrum Camera::rayImportance(const Ray &ray, Point2d *const filmPosition) const {
            // 计算新射线光栅化位置
            double step = _focal / ABS_DOT(ray.getDirection(), _front);
            Vector3d raster = ray.getOrigin() + ray.getDirection() * step - _leftBottomCorner;
            double u = DOT(raster, _right) / (DOT(_diagonalVector, _right));
            double v = DOT(raster, _up) / (DOT(_diagonalVector, _up));
            (*filmPosition) = Point2d(u * _resolutionWidth, v * _resolutionHeight);

            // 如果超出边界，则 rayImportance 为 0
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
                            (_area * math::PI * _lensRadius * _lensRadius * std::pow(cosine, 4));
            return Spectrum(weight);
        }

        void Camera::buildCameraCoordinate(float fov, float aspect) {
            const Vector3d worldUp = Vector3d(0.0f, 1.0f, 0.0f);
            // 判断 _front 方向是否与 worldUp 重叠
            if (abs(_front.x) < math::EPSILON && abs(_front.z) < math::EPSILON) {
                _right = {1.0f, 0.0f, 0.0f};
            } else {
                _right = NORMALIZE(CROSS(_front, worldUp));
            }
            _up = NORMALIZE(CROSS(_right, _front));

            _halfWindowHeight = tan(math::DEGREES_TO_RADIANS(fov / 2)) * _focal;
            _halfWindowWidth = _halfWindowHeight * aspect;
            _area = 4 * _halfWindowHeight * _halfWindowWidth;
            _leftBottomCorner = _eye + _focal * _front - _halfWindowWidth * _right - _halfWindowHeight * _up;
            _rightTopCorner = _eye + _focal * _front + _halfWindowWidth * _right + _halfWindowHeight * _up;
            _diagonalVector = _rightTopCorner - _leftBottomCorner;
        }
    }
}