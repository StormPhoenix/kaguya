//
// Created by Storm Phoenix on 2021/5/10.
//

#include <kaguya/material/PatinaMaterial.h>
#include <kaguya/core/bsdf/microfacet/BeckmannDistribution.h>
#include <kaguya/core/bsdf/microfacet/GGXDistribution.h>
#include <kaguya/core/bsdf/BXDFGlossyDiffuseReflection.h>

namespace kaguya {
    namespace material {

        using core::bsdf::BXDFGlossyDiffuseReflection;
        using core::bsdf::microfacet::GGXDistribution;
        using core::bsdf::microfacet::BeckmannDistribution;
        using core::bsdf::microfacet::MicrofacetDistribution;

        PatinaMaterial::PatinaMaterial(const Texture<Spectrum>::Ptr Kd, const Texture<Spectrum>::Ptr Ks,
                                       const Texture<Float>::Ptr alpha) :
                _Kd(Kd), _Ks(Ks), _alpha(alpha) {
            ASSERT(_alpha != nullptr, "CoatingMaterial parameter Alpha is nullptr. ");
            ASSERT(_Kd != nullptr, "CoatingMaterial parameter Kd is nullptr. ");
            ASSERT(_Ks != nullptr, "CoatingMaterial parameter Ks is nullptr. ");
        }

        void PatinaMaterial::computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                        TransportMode mode) {
            Float alpha = _alpha->evaluate(insect);
            Spectrum Rd = _Kd->evaluate(insect);
            Spectrum Rs = _Ks->evaluate(insect);

            insect.bsdf = ALLOC(memoryArena, BSDF)(insect);

            const MicrofacetDistribution *distribution = ALLOC(memoryArena, GGXDistribution)(alpha);
            insect.bsdf->addBXDF(ALLOC(memoryArena, BXDFGlossyDiffuseReflection)(Rd, Rs, distribution));
        }
    }
}