//
// Created by Storm Phoenix on 2021/5/10.
//

#ifndef KAGUYA_IMAGEREADER_H
#define KAGUYA_IMAGEREADER_H

#include <kaguya/Common.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace kaguya {
    namespace utils {
        namespace io {
            using core::RGBSpectrum;

            std::unique_ptr<RGBSpectrum[]> readImage(const std::string &filename, int *width, int *height);
        }
    }
}

#endif //KAGUYA_IMAGEREADER_H
