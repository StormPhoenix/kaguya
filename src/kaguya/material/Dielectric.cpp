//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/core/bsdf/BXDFSpecular.h>
#include <kaguya/material/Dielectric.h>

namespace kaguya {
    namespace material {

        using kaguya::core::BXDFSpecular;

        Dielectric::Dielectric(std::shared_ptr<Texture> albedo) : Dielectric(albedo, 1.5) {}

        Dielectric::Dielectric(std::shared_ptr<Texture> albedo, double refractiveIndex)
                : _refractiveIndex(refractiveIndex), _albedo(albedo) {
        }

        bool Dielectric::isSpecular() {
            return true;
        }

        std::shared_ptr<BSDF> Dielectric::bsdf(kaguya::core::Interaction &insect) {
            Spectrum albedo = _albedo->sample(insect.u, insect.v);
            std::shared_ptr<BXDFSpecular> specularBXDF = std::make_shared<BXDFSpecular>(albedo, 1.0f, _refractiveIndex);
            std::shared_ptr<BSDF> bsdf = std::make_shared<BSDF>(insect);
            bsdf->addBXDF(specularBXDF);
            return bsdf;
        }
    }
}