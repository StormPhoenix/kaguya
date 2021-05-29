//
// Created by Storm Phoenix on 2020/11/11.
//

#ifndef KAGUYA_FILMPLANE_H
#define KAGUYA_FILMPLANE_H

#include <kaguya/math/Math.h>
#include <kaguya/core/core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/parallel/AtomicFloat.h>

#include <mutex>

namespace RENDER_NAMESPACE {
    namespace tracer {

        using kaguya::core::Spectrum;
        using kaguya::core::SPECTRUM_CHANNEL;
        using kaguya::parallel::AtomicFloat;

        using namespace math;

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

        class FilmTile {
        public:
            typedef std::shared_ptr<FilmTile> Ptr;

            friend class FilmPlane;

            FilmTile(Point2I offset, int width, int height);

            void addSpectrum(const Spectrum &spectrum, int row, int col);

            void setSpectrum(const Spectrum &spectrum, int row, int col);

        private:
            int _width, _height;
            Point2I _offsetInFilm;
            std::unique_ptr<Pixel[]> _filmTile;
        };

        class FilmPlane {
        public:
            FilmPlane(int resolutionWidth, int resolutionHeight, int channel);

            void addExtra(const Spectrum &spectrum, int row, int col);

            void addSpectrum(const Spectrum &spectrum, int row, int col);

            void setSpectrum(const Spectrum &spectrum, int row, int col);

            void mergeTile(FilmTile::Ptr tile);

            void writeImage(char const *filename, Float weight = 1.0f);

        private:
            int _resolutionWidth = 0;
            int _resolutionHeight = 0;
            int _channel = 0;
            std::unique_ptr<Pixel[]> _film;
            std::mutex _writeLock;
        };

    }
}

#endif //KAGUYA_FILMPLANE_H
