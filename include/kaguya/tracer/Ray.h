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
            Ray() : _medium(nullptr) {}

            /**
             * 射线初始化
             * @param origin 射线起点
             * @param direction 射线方向
             */
            Ray(const Vector3 &origin, const Vector3 &direction,
                const core::medium::Medium *medium = nullptr);

            const Vector3 &getDirection() const;

            const void setDirection(const Vector3 &direction);

            const Vector3 &getOrigin() const;

            const void setOrigin(const Vector3 &origin);

            const void setMedium(core::medium::Medium *medium);

            const core::medium::Medium *getMedium() const;

            const Vector3 at(double step) const;

        private:
            Vector3 _origin;
            Vector3 _direction;
            const core::medium::Medium *_medium;
        };

    }
}

#endif //KAGUYA_RAY_H
