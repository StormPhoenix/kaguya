//
// Created by Storm Phoenix on 2020/11/11.
//

#include <kaguya/tracer/FilmPlane.h>

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


        unsigned int FilmPlane::getSpectrum(int row, int col, int channel) const {
            int offset = (row * _resolutionWidth + col) * _channel;
            // TODO 修改此处，用于适应 Spectrum
            // TODO 应该由外界来 clamp
            // Write the translated [0,255] value of each color component.
            return static_cast<int>(256 * clamp(std::sqrt((_bitmap + offset)[channel]), 0.0, 0.999));
        }

        FilmPlane::~FilmPlane() {
            free(_bitmap);
        }
    }
}