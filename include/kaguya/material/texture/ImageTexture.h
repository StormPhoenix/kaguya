//
// Created by Storm Phoenix on 2021/4/14.
//

#ifndef KAGUYA_IMAGETEXTURE_H
#define KAGUYA_IMAGETEXTURE_H

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/material/texture/Texture.h>
#include <kaguya/material/texture/TextureMapping2D.h>
#include <kaguya/material/texture/UVMapping2D.h>

#define STB_IMAGE_IMPLEMENTATION

#include <stb/stb_image.h>

#include <string>
#include <iostream>
#include <fstream>

namespace kaguya {
    namespace material {
        namespace texture {

#define MAX_CHANNEL 10

            template<typename T>
            class ImageTexture : public Texture<T> {
            public:
                ImageTexture(std::string imagePath, std::shared_ptr<TextureMapping2D> textureMapping = nullptr) {

                    /* Check file exists */
                    {
                        std::ifstream in(imagePath);
                        if (!in.good()) {
                            std::cout << imagePath << " NOT EXISTS." << std::endl;
                            return;
                        }
                    }

                    // Mapping
                    {
                        if (textureMapping != nullptr) {
                            _textureMapping = textureMapping;
                        } else {
                            _textureMapping = std::make_shared<UVMapping2D>();
                        }
                    }

                    _texture = stbi_load(imagePath.c_str(), &_width, &_height, &_channel, 0);
                    if (_channel > MAX_CHANNEL) {
                        std::cout << "Image channel count NOT Support !" << std::endl;
                    }
                }

                T evaluate(const SurfaceInteraction &si) override {
                    if (_texture == nullptr) {
                        return 0;
                    }
                    Point2F uv = _textureMapping->map(si);

                    int wOffset, hOffset;
                    wOffset = uv[0] * _width;
                    hOffset = uv[1] * _height;

                    if (wOffset < 0 || wOffset >= _width
                        || hOffset < 0 || hOffset >= _height) {
                        return T(0);
                    }

                    // flip
                    hOffset = _height - (hOffset + 1);
                    int offset = (hOffset * _width + wOffset) * _channel;

                    for (int ch = 0; ch < _channel && ch < MAX_CHANNEL; ch++) {
                        _value[ch] = Float(_texture[offset + ch]) / 255;
                    }
                    return mapT<T>(_value, _channel);
                }

                ~ImageTexture() {
                    if (_texture != nullptr) {
                        stbi_image_free(_texture);
                    }
                }

            private:
                template<typename _T>
                _T mapT(Float value[], int channel) {
                    _T ret(0);
                    for (int ch = 0; ch < channel && ch < ret.channels(); ch++) {
                        ret[ch] = value[ch];
                    }
                    return ret;
                }

                template<>
                Float mapT<Float>(Float value[], int channel) {
                    return value[0];
                }

            private:
                std::shared_ptr<TextureMapping2D> _textureMapping = nullptr;
                unsigned char *_texture = nullptr;
                Float _value[MAX_CHANNEL];
                int _width, _height;
                int _channel;
            };
        }
    }
}
#endif //KAGUYA_IMAGETEXTURE_H
