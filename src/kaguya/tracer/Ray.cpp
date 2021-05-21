//
// Created by Storm Phoenix on 2020/9/30.
//

#include <kaguya/tracer/Ray.h>

namespace RENDER_NAMESPACE {
    namespace tracer {

        Ray::Ray() : _medium(nullptr), _minStep(0.0), _step(math::infinity) {}

        // TODO Medium 换成 shared 指针
        Ray::Ray(const Vector3F &origin, const Vector3F &direction,
                 const Medium *medium) :
                _origin(origin), _direction(direction),
                _medium(medium), _minStep(0.0), _step(math::infinity) {}

        const Vector3F &Ray::getDirection() const {
            return _direction;
        }

        void Ray::setStep(Float step) {
            assert(step > _minStep);
            _step = step;
        }

        const Vector3F &Ray::getOrigin() const {
            return _origin;
        }

        const Vector3F Ray::at(Float step) const {
            return _origin + Vector3F(_direction.x * step, _direction.y * step, _direction.z * step);
        }
    }
}