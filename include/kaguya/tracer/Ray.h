//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_RAY_H
#define KAGUYA_RAY_H

#include <kaguya/math/Math.hpp>

namespace kaguya {
    namespace tracer {

        class Ray {
        public:
            Ray() {}

            /**
             * 射线初始化
             * @param origin 射线起点
             * @param direction 射线方向
             */
            Ray(const Vector3 &origin, const Vector3 &direction);

            const Vector3 &getDirection() const;

            const void setDirection(const Vector3 &direction);

            const Vector3 &getOrigin() const;

            const void setOrigin(const Vector3 &origin);

            const Vector3 at(double step) const;

        private:
            Vector3 _origin;
            Vector3 _direction;
        };

    }
}

#endif //KAGUYA_RAY_H
