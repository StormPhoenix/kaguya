//
// Created by Storm Phoenix on 2021/5/7.
//

#include <kaguya/material/Metal.h>
#include <kaguya/core/bsdf/microfacet/BeckmannDistribution.h>
#include <kaguya/core/bsdf/microfacet/GGXDistribution.h>
#include <kaguya/core/bsdf/BXDFMicrofacetReflection.h>
#include <kaguya/core/bsdf/fresnel/FresnelConductor.h>

namespace kaguya {
    namespace material {
        using core::bsdf::fresnel::FresnelConductor;
        using core::bsdf::BXDFMicrofacetReflection;
        using core::bsdf::microfacet::BeckmannDistribution;
        using core::bsdf::microfacet::GGXDistribution;
        using core::bsdf::microfacet::MicrofacetDistribution;

        Metal::Metal(const Texture<Float>::Ptr alpha, const Texture<Spectrum>::Ptr eta, const Texture<Spectrum>::Ptr Ks,
                     const Texture<Spectrum>::Ptr K, std::string distributionType) :
                _alpha(alpha), _eta(eta), _Ks(Ks), _K(K), _distributionType(distributionType) {
            ASSERT(_alpha != nullptr, "Alpha is nullptr. ");
            ASSERT(_eta != nullptr, "Eta is nullptr. ");
            ASSERT(_Ks != nullptr, "R is nullptr. ");
            ASSERT(_K != nullptr, "K is nullptr. ");
        }

        void Metal::computeScatteringFunctions(SurfaceInteraction &insect,
                                               MemoryArena &memoryArena,
                                               TransportMode mode) {
            // Build distribution
            const MicrofacetDistribution *distribution = nullptr;
            Float alpha = _alpha->evaluate(insect);
            if (_distributionType == "beckmann") {
                distribution = ALLOC(memoryArena, BeckmannDistribution)(alpha);
            } else if (_distributionType == "ggx") {
                distribution = ALLOC(memoryArena, GGXDistribution)(alpha);
            } else {
                ASSERT(false, "Unsupported microfacet normal distribution type. ");
            }

            // Build fresnel term
            const Spectrum etaT = _eta->evaluate(insect);
            const Spectrum K = _K->evaluate(insect);
            const FresnelConductor *fresnel = ALLOC(memoryArena, FresnelConductor)(Spectrum(1.), etaT, K);

            Spectrum reflectance = _Ks->evaluate(insect);
            insect.bsdf  = ALLOC(memoryArena, BSDF)(insect);
            insect.bsdf->addBXDF( ALLOC(memoryArena, BXDFMicrofacetReflection)(reflectance, distribution, fresnel));
        }
    }
}