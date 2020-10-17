//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/material/ConstantTexture.h>

namespace kaguya {
    namespace material {

        ConstantTexture::ConstantTexture() {
            _albedo = Spectrum(0.0);
        }

        ConstantTexture::ConstantTexture(const Spectrum &albedo) : _albedo(albedo) {}

        Spectrum ConstantTexture::sample(double u, double v) {
            return _albedo;
        }

    }
}