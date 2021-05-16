//
// Created by Storm Phoenix on 2021/5/16.
//

#include <kaguya/material/OrenNayar.h>
#include <kaguya/core/bsdf/BSDF.h>
#include <kaguya/core/bsdf/BXDFOrenNayar.h>

namespace kaguya {
    namespace material {

        using kaguya::core::bsdf::BSDF;
        using kaguya::core::bsdf::BXDFOrenNayar;

        OrenNayar::OrenNayar(Texture<Spectrum>::Ptr Kd, Texture<Float>::Ptr roughness) :
                _Kd(Kd), _roughness(roughness) {}

        void OrenNayar::computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                   TransportMode mode) {
            Spectrum Kd = _Kd->evaluate(insect);
            Float roughness = _roughness->evaluate(insect);

            BXDFOrenNayar *bxdfOrenNayar = ALLOC(memoryArena, BXDFOrenNayar)(Kd, roughness);
            insect.bsdf = ALLOC(memoryArena, BSDF)(insect);
            insect.bsdf->addBXDF(bxdfOrenNayar);
        }

    }
}