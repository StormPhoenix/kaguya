//
// Created by Storm Phoenix on 2020/9/30.
//

#include <kaguya/tracer/Ray.h>

namespace kaguya {
    namespace tracer {

        Ray::Ray() : _medium(nullptr), _minStep(0.0), _step(math::infinity) {}

        Ray::Ray(const Vector3d &origin, const Vector3d &direction,
                 const Medium *medium) :
                _origin(origin), _direction(direction),
                _medium(medium), _minStep(0.0), _step(math::infinity) {}

        const Vector3d &Ray::getDirection() const {
            return _direction;
        }

        void Ray::setStep(double step) {
            assert(step > _minStep);
            _step = step;
        }

        const Vector3d &Ray::getOrigin() const {
            return _origin;
        }

        const Vector3d Ray::at(double step) const {
            return _origin + Vector3d(_direction.x * step, _direction.y * step, _direction.z * step);
        }
    }
}