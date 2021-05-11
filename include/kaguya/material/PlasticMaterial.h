//
// Created by Storm Phoenix on 2021/5/11.
//

#ifndef KAGUYA_PLASTICMATERIAL_H
#define KAGUYA_PLASTICMATERIAL_H

#include <kaguya/material/Material.h>
#include <kaguya/material/texture/Texture.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace kaguya {
    namespace material {

        using texture::Texture;
        using core::Spectrum;

        // Diffuse + Microfacet
        class PlasticMaterial : public Material {
        public:
            PlasticMaterial(Texture<Spectrum>::Ptr Kd, Texture<Spectrum>::Ptr Ks,
                            Texture<Float>::Ptr etaI, Texture<Float>::Ptr etaT, Float alpha);

            virtual void computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                    TransportMode mode = TransportMode::RADIANCE) override;

        private:
            Texture<Spectrum>::Ptr _Kd = nullptr;
            Texture<Spectrum>::Ptr _Ks = nullptr;
            Texture<Float>::Ptr _etaI = nullptr;
            Texture<Float>::Ptr _etaT = nullptr;
            Float _alpha;
        };

    }
}

#endif //KAGUYA_PLASTICMATERIAL_H
