//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/math/HemiCosineSampler.h>
#include <kaguya/math/HemiUniformSampler.h>
#include <kaguya/material/Lambertian.h>
#include <kaguya/core/bsdf/BSDF.h>
#include <kaguya/core/bsdf/BXDFLambertianReflection.h>

namespace kaguya {
    namespace material {

        using kaguya::math::HemiCosineSampler;
        using kaguya::math::HemiUniformSampler;
        using kaguya::core::BSDF;
        using kaguya::core::BXDFLambertianReflection;

        Lambertian::Lambertian(std::shared_ptr<Texture> albedo) : _albedo(albedo) {
            _pdf = std::make_shared<HemiCosineSampler>();
//            _pdf = std::make_shared<HemiUniformSampler>();
        }

        std::shared_ptr<BSDF> Lambertian::bsdf(Interaction &insect) {
            Spectrum albedo = _albedo->sample(insect.u, insect.v);
            std::shared_ptr<BXDFLambertianReflection> lambertianBXDF =
                    std::make_shared<BXDFLambertianReflection>(albedo);

            std::shared_ptr<BSDF> bsdf = std::make_shared<BSDF>(insect);
            bsdf->addBXDF(lambertianBXDF);
            return bsdf;
        }

    }
}