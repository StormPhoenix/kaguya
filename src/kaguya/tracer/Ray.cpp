//
// Created by Storm Phoenix on 2020/9/30.
//

#include <kaguya/tracer/Ray.h>

namespace kaguya {
    namespace tracer {

        Ray::Ray(const Vector3 &origin, const Vector3 &direction) : _origin(origin), _direction(NORMALIZE(direction)) {}

        const Vector3 &Ray::getDirection() const {
            return _direction;
        }

        const void Ray::setDirection(const Vector3 &direction) {
            _direction = direction;
        }

        const Vector3 &Ray::getOrigin() const {
            return _origin;
        }

        const void Ray::setOrigin(const Vector3 &origin) {
            _origin = origin;
        }

        const Vector3 Ray::at(double step) const {
            return _origin + glm::vec3(_direction.x * step, _direction.y * step, _direction.z * step);
        }
    }
}