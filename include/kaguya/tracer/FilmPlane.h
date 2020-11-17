//
// Created by Storm Phoenix on 2020/11/11.
//

#ifndef KAGUYA_FILMPLANE_H
#define KAGUYA_FILMPLANE_H

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

#include <mutex>

namespace kaguya {
    namespace tracer {

        using kaguya::core::Spectrum;

        class FilmPlane {
        public:
            FilmPlane(int resolutionWidth, int resolutionHeight, int channel);

            void addSpectrum(const Spectrum &spectrum, int row, int col);

            double getSpectrum(int row, int col, int channel) const;

            void writeImage(char const *filename) const;

            ~FilmPlane();

        private:
            int _resolutionWidth = 0;
            int _resolutionHeight = 0;
            int _channel = 0;
            double *_bitmap = nullptr;
            std::mutex writeLock;
        };

    }
}

#endif //KAGUYA_FILMPLANE_H
