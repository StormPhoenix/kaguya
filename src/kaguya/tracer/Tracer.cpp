//
// Created by Storm Phoenix on 2020/11/2.
//

#include <kaguya/tracer/Tracer.h>

namespace kaguya {
    namespace tracer {

        void Tracer::writeShaderColor(const Spectrum &spectrum, int row, int col) {
            assert(_filmPlane != nullptr && _camera != nullptr);
            _filmPlane->addSpectrum(spectrum, row, col);
        }

    }
}