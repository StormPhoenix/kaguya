//
// Created by Storm Phoenix on 2021/5/10.
//

#ifndef KAGUYA_IMAGEIO_H
#define KAGUYA_IMAGEIO_H

#include <kaguya/common.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace RENDER_NAMESPACE {
    namespace utils {
        namespace io {
            using core::RGBSpectrum;

            std::unique_ptr<RGBSpectrum[]> readImage(const std::string &filename, int *width, int *height);
        }
    }
}

#endif //KAGUYA_IMAGEIO_H
