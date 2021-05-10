//
// Created by Storm Phoenix on 2021/5/10.
//

#ifndef KAGUYA_COATINGMATERIAL_H
#define KAGUYA_COATINGMATERIAL_H

#include <kaguya/material/Material.h>
#include <kaguya/material/texture/Texture.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace kaguya {
    namespace material {
        using namespace texture;

        class CoatingMaterial : public Material {
        public:
            CoatingMaterial(const Texture<Spectrum>::Ptr Kd,
                            const Texture<Spectrum>::Ptr Ks,
                            const Texture<Float>::Ptr alpha);

            virtual void computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                    TransportMode mode = TransportMode::RADIANCE) override;

        protected:
            const Texture<Spectrum>::Ptr _Kd;
            const Texture<Spectrum>::Ptr _Ks;
            const Texture<Float>::Ptr _alpha;
        };
    }
}

#endif //KAGUYA_COATINGMATERIAL_H
