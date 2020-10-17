//
// Created by Storm Phoenix on 2020/9/30.
//

#include <kaguya/scene/Camera.h>

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
            buildCameraCoordinate(fov, aspect);
        }

        Ray Camera::sendRay(double u, double v) {
            Vector3 samplePoint =
                    _leftBottomCorner + 2 * _halfWindowWidth * u * _right + 2 * _halfWindowHeight * v * _up;
            Vector3 dir = NORMALIZE(samplePoint - _eye);
            return Ray(_eye, dir);
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

        void Camera::buildCameraCoordinate(float fov, float aspect) {
            Vector3 worldUp = Vector3(0.0f, 1.0f, 0.0f);
            // 判断 _fron 方向是否与 worldUp 重叠
            if (abs(_front.x) < EPSILON && abs(_front.z) < EPSILON) {
                _right = {1.0f, 0.0f, 0.0f};
            } else {
                _right = NORMALIZE(CROSS(_front, worldUp));
            }
            _up = NORMALIZE(CROSS(_right, _front));

            // 默认焦距为 10
            double focal = 10;
            _halfWindowHeight = tan(DEGREES_TO_RADIANS(fov / 2)) * focal;
            _halfWindowWidth = _halfWindowHeight * aspect;
            _leftBottomCorner = _eye + focal * _front - _halfWindowWidth * _right - _halfWindowHeight * _up;
        }
    }
}