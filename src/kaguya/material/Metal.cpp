//
// Created by Storm Phoenix on 2021/5/7.
//

#include <kaguya/material/Metal.h>
#include <kaguya/core/bsdf/microfacet/BeckmannDistribution.h>
#include <kaguya/core/bsdf/BXDFMicrofacetReflection.h>
#include <kaguya/core/bsdf/FresnelConductor.h>

namespace kaguya {
    namespace material {
        using core::FresnelConductor;
        using core::BXDFMicrofacetReflection;
        using core::microfacet::BeckmannDistribution;

        Metal::Metal(const Texture<Float>::Ptr alpha, const Texture<Spectrum>::Ptr eta, const Texture<Spectrum>::Ptr R,
                     const Texture<Spectrum>::Ptr K, std::string distributionType) :
                _alpha(alpha), _eta(eta), _R(R), _K(K), _distributionType(distributionType) {
            ASSERT(_alpha != nullptr, "Alpha is nullptr. ");
            ASSERT(_eta != nullptr, "Eta is nullptr. ");
            ASSERT(_R != nullptr, "R is nullptr. ");
            ASSERT(_K != nullptr, "K is nullptr. ");
        }

        void Metal::computeScatteringFunctions(SurfaceInteraction &insect,
                                               MemoryArena &memoryArena,
                                               TransportMode mode) {
            if (_distributionType == "backmann") {
                // Distribution
                Float alpha = _alpha->evaluate(insect);
                const BeckmannDistribution *distribution = ALLOC(memoryArena, BeckmannDistribution)(alpha);

                const Spectrum thetaT = _eta->evaluate(insect);
                const Spectrum k = _K->evaluate(insect);
                const FresnelConductor *fresnel = ALLOC(memoryArena, FresnelConductor)(Spectrum(1.), thetaT, k);

                Spectrum reflectance = _R->evaluate(insect);
                BSDF *bsdf = ALLOC(memoryArena, BSDF)(insect);
                BXDF *bxdf = ALLOC(memoryArena, BXDFMicrofacetReflection)(reflectance, distribution, fresnel);
                bsdf->addBXDF(bxdf);
                insect.bsdf = bsdf;
            } else {
                ASSERT(false, "Unsupported microfacet normal distribution type. ");
            }
        }
    }
}