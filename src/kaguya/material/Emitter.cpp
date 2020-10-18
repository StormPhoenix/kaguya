//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/material/Emitter.h>
#include <kaguya/material/ConstantTexture.h>

namespace kaguya {
    namespace material {

        Emitter::Emitter() : Emitter(std::make_shared<ConstantTexture>(Spectrum(1.0))) {}

        Emitter::Emitter(std::shared_ptr<Texture> albedo) : _albedo(albedo) {}

        std::shared_ptr<BSDF> Emitter::bsdf(kaguya::core::Interaction &insect) {
            return nullptr;
        }

        Spectrum Emitter::emitted(double u, double v) {
            return _albedo->sample(u, v);
        }
    }
}
