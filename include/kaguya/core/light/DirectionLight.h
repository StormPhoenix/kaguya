//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_DIRECTIONLIGHT_H
#define KAGUYA_DIRECTIONLIGHT_H

#include <kaguya/core/light/Light.h>

namespace kaguya {
    namespace core {

        class DirectionLight : public Light {
        public:
            DirectionLight(const Vector3 origin, const Vector3 dir);
        };

    }
}

#endif //KAGUYA_DIRECTIONLIGHT_H
