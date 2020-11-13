//
// Created by Storm Phoenix on 2020/11/11.
//

#include <kaguya/tracer/FilmPlane.h>
#include <iostream>

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
            for (int channel = 0; channel < _channel; channel++) {
                *(_bitmap + offset + channel) += spectrum[channel];
            }
        }


        double FilmPlane::getSpectrum(int row, int col, int channel) const {
            int offset = (row * _resolutionWidth + col) * _channel;
            return (_bitmap + offset)[channel];
        }

        void FilmPlane::writeImage() const {
            std::cout << "P3\n" << _resolutionWidth << " " << _resolutionHeight << "\n255\n";
            // Write the translated [0,255] value of each color component.
            // TODO 更改成写入替换策略
            for (int row = _resolutionHeight - 1; row >= 0; row--) {
                for (int col = 0; col < _resolutionWidth; col++) {
                    std::cout << static_cast<int>(256 * clamp(std::sqrt(getSpectrum(row, col, 0)), 0.0, 0.999)) << ' '
                              << static_cast<int>(256 * clamp(std::sqrt(getSpectrum(row, col, 1)), 0.0, 0.999)) << ' '
                              << static_cast<int>(256 * clamp(std::sqrt(getSpectrum(row, col, 2)), 0.0, 0.999)) << '\n';
                }
            }
        }

        FilmPlane::~FilmPlane() {
            free(_bitmap);
        }
    }
}