//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/material/ConstantTexture.h>

namespace kaguya {
    namespace material {

        ConstantTexture::ConstantTexture() {
            _albedo = {0.0f, 0.0f, 0.0f};
        }

        ConstantTexture::ConstantTexture(const Vector3 &albedo) : _albedo(albedo) {}

        Vector3 ConstantTexture::sample(double u, double v) {
            return _albedo;
        }

    }
}