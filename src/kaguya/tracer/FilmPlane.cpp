//
// Created by Storm Phoenix on 2020/11/11.
//

#include <kaguya/tracer/FilmPlane.h>
#include <cstring>
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb/stb_image_write.h>

namespace kaguya {
    namespace tracer {

        FilmPlane::FilmPlane(int resolutionWidth, int resolutionHeight, int channel) :
                _resolutionWidth(resolutionWidth), _resolutionHeight(resolutionHeight),
                _channel(channel) {
            _bitmap = (double *) malloc(resolutionHeight * resolutionWidth * channel * sizeof(double));
            memset(_bitmap, 0, resolutionHeight * resolutionWidth * channel * sizeof(double));
        }

        void FilmPlane::addSpectrum(const Spectrum &spectrum, int row, int col) {
            assert(_bitmap != nullptr);
            if (row < 0 || row >= _resolutionHeight ||
                col < 0 || col >= _resolutionWidth) {
                return;
            }

            int offset = (row * _resolutionWidth + col) * _channel;
            // lock_guard will release the lock when exits stack
            {
                std::lock_guard<std::mutex> lock(writeLock);
                for (int channel = 0; channel < _channel; channel++) {
                    *(_bitmap + offset + channel) += spectrum[channel];
                }
            }
            // writeLock is automatically released when lock goes out of scope
        }


        double FilmPlane::getSpectrum(int row, int col, int channel) const {
            int offset = (row * _resolutionWidth + col) * _channel;
            return (_bitmap + offset)[channel];
        }

        void FilmPlane::writeImage(char const *filename) const {
            // 创建 image buffer
            unsigned char *image = (unsigned char *) malloc(
                    sizeof(unsigned char) * _resolutionWidth * _resolutionHeight * _channel);

            // 将光谱转化到 image buffer
            for (int row = _resolutionHeight - 1; row >= 0; row--) {
                for (int col = 0; col < _resolutionWidth; col++) {
                    int offset = ((_resolutionHeight - 1 - row) * _resolutionWidth + col) * _channel;
                    (image + offset)[0] = static_cast<unsigned char>(256 *
                                                                     clamp(std::sqrt(getSpectrum(row, col, 0)), 0.0,
                                                                           0.999));
                    (image + offset)[1] = static_cast<unsigned char>(256 *
                                                                     clamp(std::sqrt(getSpectrum(row, col, 1)), 0.0,
                                                                           0.999));
                    (image + offset)[2] = static_cast<unsigned char>(256 *
                                                                     clamp(std::sqrt(getSpectrum(row, col, 2)), 0.0,
                                                                           0.999));
                }
            }
            // 写入 image file
            stbi_write_png(filename, _resolutionWidth, _resolutionHeight, 3, image, 0);
            free(image);
        }

        FilmPlane::~FilmPlane() {
            free(_bitmap);
        }
    }
}