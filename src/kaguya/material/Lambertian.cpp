//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/material/Lambertian.h>
#include <kaguya/core/bsdf/BSDF.h>
#include <kaguya/core/bsdf/BXDFLambertianReflection.h>

namespace RENDER_NAMESPACE {
    namespace material {

        using kaguya::core::bsdf::BSDF;
        using kaguya::core::bsdf::BXDFLambertianReflection;

        Lambertian::Lambertian(std::shared_ptr<Texture<Spectrum>> Kd) : _Kd(Kd) {}

        void Lambertian::computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                    TransportMode mode) {
            Spectrum albedo = _Kd->evaluate(insect);
            BXDFLambertianReflection *lambertianBXDF =
                    ALLOC(memoryArena, BXDFLambertianReflection)(albedo);
            insect.bsdf = ALLOC(memoryArena, BSDF)(insect);
            insect.bsdf->addBXDF(lambertianBXDF);
        }

    }
}