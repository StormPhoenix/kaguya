//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_TEXTURE_H
#define KAGUYA_TEXTURE_H

#include <kaguya/core/Interaction.h>

namespace kaguya {
    namespace material {
        namespace texture {

            using namespace core;

            template<typename T>
            class Texture {
            public:
                typedef std::shared_ptr<Texture<T>> Ptr;

                virtual T evaluate(const SurfaceInteraction &si) = 0;
            };
        }
    }
}

#endif //KAGUYA_TEXTURE_H
