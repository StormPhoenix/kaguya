//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_RAY_H
#define KAGUYA_RAY_H

#include <kaguya/math/Math.h>

namespace kaguya {

    namespace core {
        namespace medium {
            class Medium;
        }
    }

    namespace tracer {

        using core::medium::Medium;

        class Ray {
        public:
            Ray();

            /**
             * 射线初始化
             * @param origin 射线起点
             * @param direction 射线方向
             */
            Ray(const Vector3F &origin, const Vector3F &direction,
                const Medium *medium = nullptr);

            const Vector3F &getDirection() const;

            const Vector3F &getOrigin() const;

            const void setMedium(Medium *medium);

            const Medium *getMedium() const {
                return _medium;
            }

            Float getMinStep() const {
                return _minStep;
            }

            Float getStep() const {
                return _step;
            }

            void setStep(Float step);

            const Vector3F at(Float step) const;

        private:
            Vector3F _origin;
            Vector3F _direction;
            Float _minStep;
            Float _step;
            const Medium *_medium;
        };

    }
}

#endif //KAGUYA_RAY_H
