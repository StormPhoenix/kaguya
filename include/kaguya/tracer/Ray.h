//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_RAY_H
#define KAGUYA_RAY_H

#include <kaguya/math/Math.hpp>

namespace kaguya {

    namespace core {
        namespace medium {
            class Medium;
        }
    }

    namespace tracer {

        class Ray {
        public:
            Ray() : _medium(nullptr), _minStep(0.001), _step(infinity) {}

            /**
             * 射线初始化
             * @param origin 射线起点
             * @param direction 射线方向
             */
            Ray(const Vector3 &origin, const Vector3 &direction,
                const core::medium::Medium *medium = nullptr);

            const Vector3 &getDirection() const;

            const Vector3 &getOrigin() const;

            const void setMedium(core::medium::Medium *medium);

            const core::medium::Medium *getMedium() const {
                return _medium;
            }

            double getMinStep() const {
                return _minStep;
            }

            double getStep() const {
                return _step;
            }

            void setStep(double step) {
                assert(step >= _minStep);
                _step = step;
            }

            const Vector3 at(double step) const;

        private:
            Vector3 _origin;
            Vector3 _direction;
            double _minStep;
            double _step;
            const core::medium::Medium *_medium;
        };

    }
}

#endif //KAGUYA_RAY_H
