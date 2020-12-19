//
// Created by Storm Phoenix on 2020/9/30.
//

#include <kaguya/tracer/Ray.h>

namespace kaguya {
    namespace tracer {

        Ray::Ray(const Vector3 &origin, const Vector3 &direction,
                 const core::medium::Medium *medium) :
                _origin(origin), _direction(NORMALIZE(direction)),
                _medium(medium), _minStep(0.0001), _step(infinity) {}

        const Vector3 &Ray::getDirection() const {
            return _direction;
        }

        const Vector3 &Ray::getOrigin() const {
            return _origin;
        }

        const Vector3 Ray::at(double step) const {
            return _origin + Vector3(_direction.x * step, _direction.y * step, _direction.z * step);
        }
    }
}