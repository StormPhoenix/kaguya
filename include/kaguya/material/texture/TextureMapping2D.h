//
// Created by Storm Phoenix on 2021/4/15.
//

#ifndef KAGUYA_TEXTUREMAPPING2D_H
#define KAGUYA_TEXTUREMAPPING2D_H

#include <kaguya/core/Interaction.h>

namespace kaguya {
    namespace material {
        namespace texture {

            class TextureMapping2D {
            public:
                virtual Point2F map(const SurfaceInteraction &si) = 0;
            };
        }
    }
}

#endif //KAGUYA_TEXTUREMAPPING2D_H
