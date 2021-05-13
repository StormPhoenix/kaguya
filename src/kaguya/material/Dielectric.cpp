//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/material/Dielectric.h>
#include <kaguya/core/bsdf/BXDFFresnelSpecular.h>
#include <kaguya/core/bsdf/BXDFMicrofacetReflection.h>
#include <kaguya/core/bsdf/BXDFMicrofacetTransmission.h>
#include <kaguya/core/bsdf/fresnel/FresnelDielectric.h>
#include <kaguya/core/bsdf/microfacet/GGXDistribution.h>

namespace kaguya {
    namespace material {

        using core::bsdf::BSDF;
        using core::bsdf::BXDFFresnelSpecular;
        using core::bsdf::BXDFMicrofacetReflection;
        using core::bsdf::BXDFMicrofacetTransmission;
        using core::bsdf::microfacet::GGXDistribution;
        using core::bsdf::fresnel::FresnelDielectric;

        Dielectric::Dielectric(const Texture<Spectrum>::Ptr R, const Texture<Spectrum>::Ptr T,
                               Float etaI, Float etaT, Float roughness) :
                _R(R), _T(T), _etaI(etaI), _etaT(etaT), _roughness(roughness) {}

        bool Dielectric::isSpecular() const {
            return _roughness == 0.f;
        }

        void Dielectric::computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                    TransportMode mode) {
            insect.bsdf = ALLOC(memoryArena, BSDF)(insect);
            Spectrum Kr = _R->evaluate(insect);
            Spectrum Kt = _T->evaluate(insect);

            if (Kr.isBlack() && Kt.isBlack()) {
                return;
            }

            if (isSpecular()) {
                BXDFFresnelSpecular *specularBXDF = ALLOC(memoryArena, BXDFFresnelSpecular)(Kr, Kt, _etaI, _etaT, mode);
                insect.bsdf->addBXDF(specularBXDF);
            } else {
                const GGXDistribution *distribution = ALLOC(memoryArena, GGXDistribution)(_roughness);
                if (!Kr.isBlack()) {
                    const FresnelDielectric *fresnel = ALLOC(memoryArena, FresnelDielectric)(_etaI, _etaT);
                    insect.bsdf->addBXDF(ALLOC(memoryArena, BXDFMicrofacetReflection)(Kr, distribution, fresnel));
                }

                if (!Kt.isBlack()) {
                    insect.bsdf->addBXDF(
                            ALLOC(memoryArena, BXDFMicrofacetTransmission)(_etaI, _etaT, Kt, distribution, mode));
                }
            }
        }
    }
}