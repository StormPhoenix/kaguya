//
// Created by Storm Phoenix on 2021/4/14.
//

#include <kaguya/material/texture/ImageTexture.h>

#define STB_IMAGE_IMPLEMENTATION

#include <stb/stb_image.h>

namespace kaguya {
    namespace material {

        // TODO 考虑纹理 channel 数目

        ImageTexture::ImageTexture(std::string imagePath) {
            // TODO Check image exists
            _texture = stbi_load(imagePath.c_str(), &_width, &_height, &_channel, 0);
        }

        Spectrum ImageTexture::sample(Float u, Float v) {
            int wOffset, hOffset;
            wOffset = u * _width;
            hOffset = v * _height;

            if (wOffset < 0 || wOffset >= _width
                || hOffset < 0 || hOffset >= _height) {
                return Spectrum(0);
            }

            // flip
            hOffset = _height - (hOffset + 1);

            int offset = (hOffset * _width + wOffset) * _channel;
            Spectrum ret(0);
            for (int ch = 0; ch < 3; ch++) {
                ret[ch] = Float(_texture[offset + ch]) / 255;
            }
            return ret;
        }

        ImageTexture::~ImageTexture() {
            if (_texture != nullptr) {
                stbi_image_free(_texture);
            }
        }
    }
}