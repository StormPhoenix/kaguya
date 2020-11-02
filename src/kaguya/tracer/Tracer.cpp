//
// Created by Storm Phoenix on 2020/11/2.
//

#include <kaguya/tracer/Tracer.h>

namespace kaguya {
    namespace tracer {

        void Tracer::writeShaderColor(const Spectrum &spectrum, int row, int col) {
            assert(_bitmap != nullptr && _camera != nullptr);
            int offset = (row * _camera->getResolutionWidth() + col) * SPECTRUM_CHANNEL;
            for (int channel = 0; channel < SPECTRUM_CHANNEL; channel++) {
                *(_bitmap + offset + channel) = static_cast<int>(256 * clamp(std::sqrt(spectrum[channel]), 0.0, 0.999));
            }
        }

    }
}