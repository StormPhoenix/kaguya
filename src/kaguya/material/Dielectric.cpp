//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/core/bsdf/BXDFSpecular.h>
#include <kaguya/material/Dielectric.h>

namespace kaguya {
    namespace material {

        using kaguya::core::BXDFSpecular;

        Dielectric::Dielectric(std::shared_ptr<Texture<Spectrum>> albedo) : Dielectric(albedo, 1.5) {}

        Dielectric::Dielectric(std::shared_ptr<Texture<Spectrum>> albedo, Float refractiveIndex)
                : _refractiveIndex(refractiveIndex), _albedo(albedo) {
        }

        bool Dielectric::isSpecular() const {
            return true;
        }

        void Dielectric::computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena, TransportMode mode) {
            Spectrum albedo = _albedo->evaluate(insect);
            BXDFSpecular *specularBXDF = ALLOC(memoryArena, BXDFSpecular)(albedo, 1.0f, _refractiveIndex, mode);
            insect.bsdf = ALLOC(memoryArena, BSDF)(insect);
            insect.bsdf->addBXDF(specularBXDF);
        }
    }
}