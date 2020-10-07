//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_TEXTURE_H
#define KAGUYA_TEXTURE_H

#include <kaguya/math/Math.hpp>

namespace kaguya {
    namespace material {

        class Texture {
        public:
            /**
             * 纹理采样
             * @param u 纹理坐标 - x
             * @param v 纹理坐标 - y
             * @return
             */
            virtual Vector3 sample(double u, double v) = 0;
        };

    }
}

#endif //KAGUYA_TEXTURE_H
