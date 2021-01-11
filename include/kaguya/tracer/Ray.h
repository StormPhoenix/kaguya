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
            Ray(const Vector3d &origin, const Vector3d &direction,
                const Medium *medium = nullptr);

            const Vector3d &getDirection() const;

            const Vector3d &getOrigin() const;

            const void setMedium(Medium *medium);

            const Medium *getMedium() const {
                return _medium;
            }

            double getMinStep() const {
                return _minStep;
            }

            double getStep() const {
                return _step;
            }

            void setStep(double step);

            const Vector3d at(double step) const;

        private:
            Vector3d _origin;
            Vector3d _direction;
            double _minStep;
            double _step;
            const Medium *_medium;
        };

    }
}

#endif //KAGUYA_RAY_H
