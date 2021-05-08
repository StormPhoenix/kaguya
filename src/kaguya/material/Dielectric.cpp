//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/core/bsdf/BXDFSpecular.h>
#include <kaguya/material/Dielectric.h>

namespace kaguya {
    namespace material {

        using core::bsdf::BSDF;
        using core::bsdf::BXDFSpecular;

        Dielectric::Dielectric(std::shared_ptr<Texture<Spectrum>> albedo) : Dielectric(albedo, 1.5) {}

        Dielectric::Dielectric(std::shared_ptr<Texture<Spectrum>> albedo, Float refractiveIndex)
                : _thetaI(1.), _thetaT(refractiveIndex), _albedo(albedo) {}

        Dielectric::Dielectric(std::shared_ptr<Texture<Spectrum>> albedo, Float thetaI, Float thetaT)
                : _thetaI(thetaI), _thetaT(thetaT), _albedo(albedo) {}

        bool Dielectric::isSpecular() const {
            return true;
        }

        void Dielectric::computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                    TransportMode mode) {
            Spectrum albedo = _albedo->evaluate(insect);
            BXDFSpecular *specularBXDF = ALLOC(memoryArena, BXDFSpecular)(albedo, _thetaI, _thetaT, mode);
            insect.bsdf = ALLOC(memoryArena, BSDF)(insect);
            insect.bsdf->addBXDF(specularBXDF);
        }
    }
}