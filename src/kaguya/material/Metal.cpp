//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/core/bsdf/BXDFSpecularReflection.h>
#include <kaguya/core/bsdf/FresnelDefault.h>
#include <kaguya/material/Metal.h>
#include <kaguya/math/HemiReflectSampler.h>

namespace kaguya {
    namespace material {

        using kaguya::math::HemiReflectSampler;
        using kaguya::core::BXDFSpecularReflection;
        using kaguya::core::FresnelDefault;

        Metal::Metal() : Metal(Spectrum(1.0f), 0) {}

        Metal::Metal(const Spectrum &albedo, double fuzzy) : _albedo(albedo), _fuzzy(fuzzy) {
        }

        bool Metal::isSpecular() {
            return true;
        }

        BSDF *Metal::bsdf(SurfaceInteraction &insect, MemoryArena &memoryArena, TransportMode mode) {
            FresnelDefault *fresnel = ALLOC(memoryArena, FresnelDefault)();
            BXDFSpecularReflection *bxdf = ALLOC(memoryArena, BXDFSpecularReflection)(_albedo, fresnel);
            BSDF *bsdf = ALLOC(memoryArena, BSDF)(insect);
            bsdf->addBXDF(bxdf);
            return bsdf;
        }
    }
}