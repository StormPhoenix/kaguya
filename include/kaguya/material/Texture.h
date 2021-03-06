//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_TEXTURE_H
#define KAGUYA_TEXTURE_H

#include <kaguya/core/Core.h>
#include <kaguya/core/Interaction.h>
#include <kaguya/math/Math.h>

namespace kaguya {
    namespace material {

        using kaguya::core::Spectrum;
        using kaguya::core::SurfaceInteraction;

        class Texture {
        public:
            /**
             * TODO parameter type -> SurfaceInteraction &
             * TODO Spectrum -> T
             * 纹理采样
             * @param u 纹理坐标 - x
             * @param v 纹理坐标 - y
             * @return
             */
            virtual Spectrum sample(Float u, Float v) = 0;

            /**
             * TODO Spectrum -> T
             * @param si
             * @return
             */
            Spectrum evaluate(SurfaceInteraction &si) { return 0; }
        };

    }
}

#endif //KAGUYA_TEXTURE_H
