//
// Created by Storm Phoenix on 2021/4/14.
//

#ifndef KAGUYA_IMAGETEXTURE_H
#define KAGUYA_IMAGETEXTURE_H

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/utils/IOReader.h>
#include <kaguya/material/texture/Texture.h>
#include <kaguya/material/texture/TextureMapping2D.h>
#include <kaguya/material/texture/UVMapping2D.h>

namespace kaguya {
    namespace material {
        namespace texture {

#define MAX_CHANNEL 10
            using namespace utils;
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

                    _texture = io::readImage(imagePath.c_str(), &_width, &_height, &_channel, 0);
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

                    T ret(0);
                    for (int ch = 0; ch < _channel && ch < MAX_CHANNEL; ch++) {
                        ret[ch] = Float(_texture[offset + ch]) / 255;
                    }
                    return ret;
                }

                ~ImageTexture() {
                    if (_texture != nullptr) {
                        io::freeImage(_texture);
                    }
                }

            private:
                std::shared_ptr<TextureMapping2D> _textureMapping = nullptr;
                unsigned char *_texture = nullptr;
                Float _value[MAX_CHANNEL];
                int _width, _height;
                int _channel;
            };

            template<>
            Float ImageTexture<Float>::evaluate(const SurfaceInteraction &si) {
                if (_texture == nullptr) {
                    return 0;
                }
                Point2F uv = _textureMapping->map(si);

                int wOffset, hOffset;
                wOffset = uv[0] * _width;
                hOffset = uv[1] * _height;

                if (wOffset < 0 || wOffset >= _width
                    || hOffset < 0 || hOffset >= _height) {
                    return 0;
                }

                // flip
                hOffset = _height - (hOffset + 1);
                int offset = (hOffset * _width + wOffset) * _channel;

                int ch = 0;
                return Float(_texture[offset + ch]) / 255;
            }

        }
    }
}
#endif //KAGUYA_IMAGETEXTURE_H
