//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_DIELECTRIC_H
#define KAGUYA_DIELECTRIC_H

#include <kaguya/common.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/material/texture/Texture.h>

namespace RENDER_NAMESPACE {
    namespace material {

        using namespace texture;
        using namespace core;

        class Dielectric {
        public:
            Dielectric(const Texture<Spectrum>::Ptr R, const Texture<Spectrum>::Ptr T,
                       Float etaI, Float etaT, Float roughness = 0.f);

            RENDER_CPU_GPU bool isSpecular();

            RENDER_CPU_GPU void evaluateBSDF(SurfaceInteraction &insect, MemoryAllocator &allocator,
                                             TransportMode mode = TransportMode::RADIANCE);

            RENDER_CPU_GPU bool isTwoSided() const {
                return _twoSided;
            }

            RENDER_CPU_GPU void setTwoSided(bool twoSided) {
                _twoSided = twoSided;
            }

        private:
            bool _twoSided = true;
            Float _roughness;
            Float _etaI, _etaT;
            const Texture<Spectrum>::Ptr _R;
            const Texture<Spectrum>::Ptr _T;
        };

    }
}

#endif //KAGUYA_DIELECTRIC_H
