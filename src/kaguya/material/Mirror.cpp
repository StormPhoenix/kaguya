//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/core/bsdf/BSDF.h>
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
        }

        Mirror::Mirror(std::shared_ptr<Texture<Spectrum>> albedo)
                : _albedo(albedo) {}

        void
        Mirror::evaluateBSDF(SurfaceInteraction &insect, MemoryAllocator &allocator, TransportMode mode) {
            FresnelDefault *fresnel = allocator.newObject<FresnelDefault>();
            BXDFSpecularReflection *bxdf = allocator.newObject<BXDFSpecularReflection>(
                    _albedo->evaluate(insect), fresnel);
            insect.bsdf = allocator.newObject<BSDF>(insect);
            insect.bsdf->addBXDF(bxdf);
        }
    }
}