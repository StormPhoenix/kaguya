//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_DIELECTRIC_H
#define KAGUYA_DIELECTRIC_H

#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/material/Material.h>
#include <kaguya/material/texture/Texture.h>

namespace kaguya {
    namespace material {

        using namespace texture;
        using namespace core;

        class Dielectric : public Material {
        public:
            Dielectric(const Texture<Spectrum>::Ptr R, const Texture<Spectrum>::Ptr T,
                       Float etaI, Float etaT, Float roughness = 0.f);

            virtual bool isSpecular() const override;

            virtual void computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                    TransportMode mode = TransportMode::RADIANCE) override;


        private:
            Float _roughness;
            Float _etaI, _etaT;
            const Texture<Spectrum>::Ptr _R;
            const Texture<Spectrum>::Ptr _T;
        };

    }
}

#endif //KAGUYA_DIELECTRIC_H
