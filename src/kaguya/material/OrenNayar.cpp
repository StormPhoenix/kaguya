//
// Created by Storm Phoenix on 2021/5/16.
//

#include <kaguya/material/OrenNayar.h>
#include <kaguya/core/bsdf/BSDF.h>
#include <kaguya/core/bsdf/BXDFOrenNayar.h>

namespace RENDER_NAMESPACE {
    namespace material {

        using kaguya::core::bsdf::BSDF;
        using kaguya::core::bsdf::BXDFOrenNayar;

        OrenNayar::OrenNayar(Texture<Spectrum>::Ptr Kd, Texture<Float>::Ptr roughness) :
                _Kd(Kd), _roughness(roughness) {}

        void OrenNayar::evaluateBSDF(SurfaceInteraction &insect, MemoryAllocator &allocator,
                                     TransportMode mode) {
            Spectrum Kd = _Kd->evaluate(insect);
            Float roughness = _roughness->evaluate(insect);

            BXDFOrenNayar *bxdfOrenNayar =  allocator.newObject<BXDFOrenNayar>(Kd, roughness);
            insect.bsdf =  allocator.newObject<BSDF>(insect);
            insect.bsdf->addBXDF(bxdfOrenNayar);
        }

    }
}