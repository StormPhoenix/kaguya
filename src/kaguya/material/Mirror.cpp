//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/core/bsdf/BXDFSpecularReflection.h>
#include <kaguya/core/bsdf/fresnel/FresnelDefault.h>
#include <kaguya/material/Mirror.h>
#include <kaguya/material/texture/ConstantTexture.h>

namespace RENDER_NAMESPACE {
    namespace material {

        using core::bsdf::BSDF;
        using core::bsdf::BXDFSpecularReflection;
        using core::bsdf::fresnel::FresnelDefault;

        Mirror::Mirror() {
            _albedo = std::make_shared<ConstantTexture<Spectrum>>(Spectrum(1.0));
            _fuzzy = 0;
        }

        Mirror::Mirror(std::shared_ptr<Texture<Spectrum>> albedo, Float fuzzy)
                : _albedo(albedo), _fuzzy(fuzzy) {}

        bool Mirror::isSpecular() const {
            return true;
        }

        void
        Mirror::computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena, TransportMode mode) {
            FresnelDefault *fresnel = ALLOC(memoryArena, FresnelDefault)();
            BXDFSpecularReflection *bxdf = ALLOC(memoryArena, BXDFSpecularReflection)(
                    _albedo->evaluate(insect), fresnel);
            insect.bsdf = ALLOC(memoryArena, BSDF)(insect);
            insect.bsdf->addBXDF(bxdf);
        }
    }
}