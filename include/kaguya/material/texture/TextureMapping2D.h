//
// Created by Storm Phoenix on 2021/4/15.
//

#ifndef KAGUYA_TEXTUREMAPPING2D_H
#define KAGUYA_TEXTUREMAPPING2D_H

#include <kaguya/core/Interaction.h>

namespace RENDER_NAMESPACE {
    namespace material {
        namespace texture {

            class TextureMapping2D {
            public:
                typedef std::shared_ptr<TextureMapping2D> Ptr;

                virtual Point2F map(const SurfaceInteraction &si) = 0;
            };
        }
    }
}

#endif //KAGUYA_TEXTUREMAPPING2D_H
