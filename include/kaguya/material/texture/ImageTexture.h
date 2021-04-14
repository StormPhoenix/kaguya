//
// Created by Storm Phoenix on 2021/4/14.
//

#ifndef KAGUYA_IMAGETEXTURE_H
#define KAGUYA_IMAGETEXTURE_H

#include <kaguya/core/Core.h>
#include <kaguya/material/texture/Texture.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

#include <string>

namespace kaguya {
    namespace material {

        class ImageTexture : public Texture {
        public:
            ImageTexture(std::string imagePath);

            Spectrum sample(Float u, Float v) override;

            ~ImageTexture();

        private:
            unsigned char *_texture = nullptr;
            int _width, _height;
            int _channel;
        };

    }
}
#endif //KAGUYA_IMAGETEXTURE_H
