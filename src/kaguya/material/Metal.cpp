//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/core/bsdf/BXDFSpecularReflection.h>
#include <kaguya/core/bsdf/FresnelDefault.h>
#include <kaguya/material/Metal.h>
#include <kaguya/material/texture/ConstantTexture.h>

namespace kaguya {
    namespace material {

        using kaguya::core::BXDFSpecularReflection;
        using kaguya::core::FresnelDefault;

        Metal::Metal() {
            _albedo = std::make_shared<ConstantTexture<Spectrum>>(Spectrum(1.0));
            _fuzzy = 0;
        }

        Metal::Metal(std::shared_ptr<Texture<Spectrum>> albedo, Float fuzzy)
                : _albedo(albedo), _fuzzy(fuzzy) {}

        bool Metal::isSpecular() const {
            return true;
        }

        void
        Metal::computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena, TransportMode mode) {
            FresnelDefault *fresnel = ALLOC(memoryArena, FresnelDefault)();
            BXDFSpecularReflection *bxdf = ALLOC(memoryArena, BXDFSpecularReflection)(
                    _albedo->evaluate(insect), fresnel);
            insect.bsdf = ALLOC(memoryArena, BSDF)(insect);
            insect.bsdf->addBXDF(bxdf);
        }
    }
}