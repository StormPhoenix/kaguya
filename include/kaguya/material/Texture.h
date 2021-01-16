//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_TEXTURE_H
#define KAGUYA_TEXTURE_H

#include <kaguya/core/Core.h>
#include <kaguya/math/Math.h>

namespace kaguya {
    namespace material {

        using kaguya::core::Spectrum;

        class Texture {
        public:
            /**
             * 纹理采样
             * @param u 纹理坐标 - x
             * @param v 纹理坐标 - y
             * @return
             */
            virtual Spectrum sample(Float u, Float v) = 0;
        };

    }
}

#endif //KAGUYA_TEXTURE_H
