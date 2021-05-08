//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/core/bsdf/BXDFSpecular.h>
#include <kaguya/material/Dielectric.h>

namespace kaguya {
    namespace material {

        using core::bsdf::BSDF;
        using core::bsdf::BXDFSpecular;

        Dielectric::Dielectric(const Texture<Spectrum>::Ptr R, const Texture<Spectrum>::Ptr T,
                               Float etaI, Float etaT) : _R(R), _T(T), _etaI(etaI), _etaT(etaT) {}

        bool Dielectric::isSpecular() const {
            return true;
        }

        void Dielectric::computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                    TransportMode mode) {
            Spectrum R = _R->evaluate(insect);
            Spectrum T = _T->evaluate(insect);
            BXDFSpecular *specularBXDF = ALLOC(memoryArena, BXDFSpecular)(R, T, _etaI, _etaT, mode);
            insect.bsdf = ALLOC(memoryArena, BSDF)(insect);
            insect.bsdf->addBXDF(specularBXDF);
        }
    }
}