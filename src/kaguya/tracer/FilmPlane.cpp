//
// Created by Storm Phoenix on 2020/11/11.
//

#include <kaguya/tracer/FilmPlane.h>
#include <cstring>
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <ext/stb/stb_image_write.h>

namespace kaguya {
    namespace tracer {

        FilmPlane::FilmPlane(int resolutionWidth, int resolutionHeight, int channel) :
                _resolutionWidth(resolutionWidth), _resolutionHeight(resolutionHeight),
                _channel(channel) {
            int filmSize = _resolutionWidth * _resolutionHeight;
            _film = std::unique_ptr<Pixel[]>(new Pixel[filmSize]);
        }

        void FilmPlane::addExtra(const Spectrum &spectrum, int row, int col) {
            if (row < 0 || row >= _resolutionHeight ||
                col < 0 || col >= _resolutionWidth) {
                return;
            }

            int offset = (row * _resolutionWidth + col);
            Pixel &pixel = _film[offset];
            for (int ch = 0; ch < _channel; ch++) {
                pixel.extra[ch].add(spectrum[ch]);
            }
        }

        void FilmPlane::addSpectrum(const Spectrum &spectrum, int row, int col) {
            assert(_film != nullptr);
            if (row < 0 || row >= _resolutionHeight ||
                col < 0 || col >= _resolutionWidth) {
                return;
            }

            int offset = (row * _resolutionWidth + col);
            Pixel &pixel = _film[offset];
            pixel.spectrum += spectrum;
        }

        void FilmPlane::setSpectrum(const Spectrum &spectrum, int row, int col) {
            assert(_film != nullptr);
            if (row < 0 || row >= _resolutionHeight ||
                col < 0 || col >= _resolutionWidth) {
                return;
            }

            int offset = (row * _resolutionWidth + col);
            Pixel &pixel = _film[offset];
            pixel.spectrum = spectrum;
        }

        void FilmPlane::writeImage(char const *filename) {
            // Create image buffer
            unsigned char *image = new unsigned char[_resolutionWidth * _resolutionHeight * _channel];
            {
                std::lock_guard<std::mutex> lock(writeLock);
                // 将光谱转化到 image buffer
                for (int row = _resolutionHeight - 1; row >= 0; row--) {
                    for (int col = 0; col < _resolutionWidth; col++) {
                        int imageOffset = ((_resolutionHeight - 1 - row) * _resolutionWidth + col) * _channel;
                        int pixelOffset = row * _resolutionWidth + col;
                        Pixel &pixel = _film[pixelOffset];
                        for (int ch = 0; ch < _channel; ch++) {
                            (image + imageOffset)[ch] = static_cast<unsigned char>(
                                    256 * math::clamp(std::sqrt(pixel.spectrum[ch] + pixel.extra[ch].get()), 0.0, 0.999));
                        }
                    }
                }
            }
            // 写入 image file
            stbi_write_png(filename, _resolutionWidth, _resolutionHeight, 3, image, 0);
            free(image);
        }
    }
}