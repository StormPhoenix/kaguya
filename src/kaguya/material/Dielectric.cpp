//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/material/Dielectric.h>
#include <kaguya/core/bsdf/BXDFFresnelSpecular.h>
#include <kaguya/core/bsdf/BXDFMicrofacet.h>
#include <kaguya/core/bsdf/BXDFMicrofacetReflection.h>
#include <kaguya/core/bsdf/BXDFMicrofacetTransmission.h>
#include <kaguya/core/bsdf/fresnel/FresnelDielectric.h>
#include <kaguya/core/bsdf/microfacet/GGXDistribution.h>

namespace RENDER_NAMESPACE {
    namespace material {

        using core::bsdf::BSDF;
        using core::bsdf::BXDFFresnelSpecular;
        using core::bsdf::BXDFMicrofacetReflection;
        using core::bsdf::BXDFMicrofacetTransmission;
        using core::bsdf::BXDFMicrofacet;
        using core::bsdf::microfacet::GGXDistribution;
        using core::bsdf::fresnel::FresnelDielectric;

        Dielectric::Dielectric(const Texture<Spectrum>::Ptr R, const Texture<Spectrum>::Ptr T,
                               Float etaI, Float etaT, Float roughness) :
                _R(R), _T(T), _etaI(etaI), _etaT(etaT), _roughness(roughness) {}

        bool Dielectric::isSpecular() const {
            return _roughness == 0.f;
        }

        void Dielectric::computeScatteringFunctions(SurfaceInteraction &insect, MemoryAllocator &allocator,
                                                    TransportMode mode) {
            insect.bsdf = allocator.newObject<BSDF>(insect);
            Spectrum Kr = _R->evaluate(insect);
            Spectrum Kt = _T->evaluate(insect);

            if (Kr.isBlack() && Kt.isBlack()) {
                return;
            }

            if (isSpecular()) {
                BXDFFresnelSpecular *specularBXDF = allocator.newObject<BXDFFresnelSpecular>(Kr, Kt, _etaI, _etaT, mode);
                insect.bsdf->addBXDF(specularBXDF);
            } else {
                const GGXDistribution *distribution = allocator.newObject<GGXDistribution>(_roughness);
                insect.bsdf->addBXDF(allocator.newObject<BXDFMicrofacet>(Kr, Kt, _etaI, _etaT, distribution, mode));
            }
        }
    }
}