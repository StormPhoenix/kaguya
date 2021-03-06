//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/material/Lambertian.h>
#include <kaguya/core/bsdf/BSDF.h>
#include <kaguya/core/bsdf/BXDFLambertianReflection.h>

namespace kaguya {
    namespace material {

        using kaguya::core::BSDF;
        using kaguya::core::BXDFLambertianReflection;

        Lambertian::Lambertian(std::shared_ptr<Texture> albedo) : _albedo(albedo) {}

        void Lambertian::computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena, TransportMode mode) {
            Spectrum albedo = _albedo->sample(insect.u, insect.v);
            BXDFLambertianReflection *lambertianBXDF =
                    ALLOC(memoryArena, BXDFLambertianReflection)(albedo);
            insect.bsdf = ALLOC(memoryArena, BSDF)(insect);
            insect.bsdf->addBXDF(lambertianBXDF);
        }

    }
}