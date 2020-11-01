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
        }

        BSDF *Lambertian::bsdf(SurfaceInteraction &insect, MemoryArena &memoryArena, TransportMode mode) {
            Spectrum albedo = _albedo->sample(insect.u, insect.v);
            BXDFLambertianReflection *lambertianBXDF =
                    ALLOC(memoryArena, BXDFLambertianReflection)(albedo);
            BSDF *bsdf = ALLOC(memoryArena, BSDF)(insect);
            bsdf->addBXDF(lambertianBXDF);
            return bsdf;
        }

    }
}