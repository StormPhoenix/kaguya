//
// Created by Storm Phoenix on 2021/5/11.
//

#include <kaguya/core/bsdf/BSDF.h>
#include <kaguya/material/PlasticMaterial.h>
#include <kaguya/core/bsdf/BXDFLambertianReflection.h>
#include <kaguya/core/bsdf/BXDFMicrofacetReflection.h>
#include <kaguya/core/bsdf/microfacet/BeckmannDistribution.h>
#include <kaguya/core/bsdf/fresnel/FresnelDielectric.h>

namespace RENDER_NAMESPACE {
    namespace material {

        using core::bsdf::BSDF;
        using core::bsdf::BXDFLambertianReflection;
        using core::bsdf::BXDFMicrofacetReflection;
        using core::bsdf::fresnel::Fresnel;
        using core::bsdf::fresnel::FresnelDielectric;
        using core::bsdf::microfacet::MicrofacetDistribution;
        using core::bsdf::microfacet::BeckmannDistribution;

        // Mitsuba: intIOR extIOR Kd
        // PBRT: Kd Ks Roughness
        PlasticMaterial::PlasticMaterial(Texture<Spectrum>::Ptr Kd, Texture<Spectrum>::Ptr Ks,
                                         Texture<Float>::Ptr etaI, Texture<Float>::Ptr etaT, Float alpha) :
                _Kd(Kd), _Ks(Ks), _etaI(etaI), _etaT(etaT), _alpha(alpha) {}

        void PlasticMaterial::evaluateBSDF(SurfaceInteraction &insect, MemoryAllocator &allocator,
                                           TransportMode mode) {
            insect.bsdf =  allocator.newObject<BSDF>(insect);

            // Evaluate diffuse material
            Spectrum Kd = _Kd->evaluate(insect);
            if (!Kd.isBlack()) {
                insect.bsdf->addBXDF( allocator.newObject<BXDFLambertianReflection>(Kd));
            }

            // Evaluate specular material
            Spectrum Ks = _Ks->evaluate(insect);
            if (!Ks.isBlack()) {
                // Distribution
                MicrofacetDistribution *distribution =  allocator.newObject<BeckmannDistribution>(_alpha);
                // Fresnel
                Float etaI = _etaI->evaluate(insect);
                Float etaT = _etaT->evaluate(insect);
                Fresnel *fresnel =  allocator.newObject<FresnelDielectric>(etaI, etaT);
                // Microfacet distribution
                insect.bsdf->addBXDF( allocator.newObject<BXDFMicrofacetReflection>(Ks, distribution, fresnel));
            }
        }
    }
}