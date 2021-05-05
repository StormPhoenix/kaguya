//
// Created by Storm Phoenix on 2020/11/11.
//

#ifndef KAGUYA_FILMPLANE_H
#define KAGUYA_FILMPLANE_H

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/parallel/AtomicFloat.h>

#include <mutex>

namespace kaguya {
    namespace tracer {

        using kaguya::core::Spectrum;
        using kaguya::core::SPECTRUM_CHANNEL;
        using kaguya::parallel::AtomicFloat;

        struct Pixel {
            Spectrum spectrum;
            AtomicFloat extra[SPECTRUM_CHANNEL];

            Pixel() {
                spectrum = Spectrum(0);
                for (int i = 0; i < SPECTRUM_CHANNEL; i++) {
                    extra[i] = 0;
                }
            }
        };

        class FilmPlane {
        public:
            FilmPlane(int resolutionWidth, int resolutionHeight, int channel);

            void addExtra(const Spectrum &spectrum, int row, int col);

            void addSpectrum(const Spectrum &spectrum, int row, int col);

            void setSpectrum(const Spectrum &spectrum, int row, int col);

            void writeImage(char const *filename);

        private:
            int _resolutionWidth = 0;
            int _resolutionHeight = 0;
            int _channel = 0;
            std::unique_ptr<Pixel[]> _film;
            std::mutex writeLock;
        };

    }
}

#endif //KAGUYA_FILMPLANE_H
