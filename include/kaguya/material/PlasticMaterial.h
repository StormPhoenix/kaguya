//
// Created by Storm Phoenix on 2021/5/11.
//

#ifndef KAGUYA_PLASTICMATERIAL_H
#define KAGUYA_PLASTICMATERIAL_H

#include <kaguya/material/Material.h>
#include <kaguya/material/texture/Texture.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace RENDER_NAMESPACE {
    namespace material {

        using texture::Texture;
        using core::Spectrum;

        // Diffuse + Microfacet
        class PlasticMaterial {
        public:
            PlasticMaterial(Texture<Spectrum>::Ptr Kd, Texture<Spectrum>::Ptr Ks,
                            Texture<Float>::Ptr etaI, Texture<Float>::Ptr etaT, Float alpha);

            RENDER_CPU_GPU void evaluateBSDF(SurfaceInteraction &si, MemoryAllocator &allocator,
                                             TransportMode mode = TransportMode::RADIANCE);

            RENDER_CPU_GPU bool isSpecular() {
                return false;
            }

            RENDER_CPU_GPU bool isTwoSided() {
                return _twoSided;
            }

            RENDER_CPU_GPU void setTwoSided(bool twoSided) {
                _twoSided = twoSided;
            }

        private:
            bool _twoSided = true;
            Texture<Spectrum>::Ptr _Kd = nullptr;
            Texture<Spectrum>::Ptr _Ks = nullptr;
            Texture<Float>::Ptr _etaI = nullptr;
            Texture<Float>::Ptr _etaT = nullptr;
            Float _alpha;
        };

    }
}

#endif //KAGUYA_PLASTICMATERIAL_H
