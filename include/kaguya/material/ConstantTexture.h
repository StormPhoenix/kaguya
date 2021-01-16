//
// Created by Storm Phoenix on 2020/10/1.
//

#ifndef KAGUYA_CONSTANTTEXTURE_H
#define KAGUYA_CONSTANTTEXTURE_H

#include <kaguya/material/Texture.h>
#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace kaguya {
    namespace material {

        class ConstantTexture : public Texture {
        public:
            ConstantTexture();

            ConstantTexture(const Spectrum &albedo);

            Spectrum sample(Float u, Float v) override;

        private:
            Spectrum _albedo;
        };

    }
}

#endif //KAGUYA_CONSTANTTEXTURE_H
