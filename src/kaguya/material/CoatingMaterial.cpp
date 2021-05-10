//
// Created by Storm Phoenix on 2021/5/10.
//

#include <kaguya/material/CoatingMaterial.h>
#include <kaguya/core/bsdf/microfacet/BeckmannDistribution.h>
#include <kaguya/core/bsdf/BXDFFresnelMicrofacetReflection.h>

namespace kaguya {
    namespace material {

        using core::bsdf::BXDFFresnelMicrofacetReflection;
        using core::bsdf::microfacet::BeckmannDistribution;

        CoatingMaterial::CoatingMaterial(const Texture<Spectrum>::Ptr Kd, const Texture<Spectrum>::Ptr Ks,
                                         const Texture<Float>::Ptr alpha) :
                _Kd(Kd), _Ks(Ks), _alpha(alpha) {
            ASSERT(_alpha != nullptr, "CoatingMaterial parameter Alpha is nullptr. ");
            ASSERT(_Kd != nullptr, "CoatingMaterial parameter Kd is nullptr. ");
            ASSERT(_Ks != nullptr, "CoatingMaterial parameter Ks is nullptr. ");
        }

        void CoatingMaterial::computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                         TransportMode mode) {
            Float alpha = _alpha->evaluate(insect);
            Spectrum Rd = _Kd->evaluate(insect);
            Spectrum Rs = _Ks->evaluate(insect);

            insect.bsdf = ALLOC(memoryArena, BSDF)(insect);

            // TODO default beckmann
            const BeckmannDistribution *distribution = ALLOC(memoryArena, BeckmannDistribution)(alpha);
            insect.bsdf->addBXDF(ALLOC(memoryArena, BXDFFresnelMicrofacetReflection)(Rd, Rs, distribution));
        }
    }
}