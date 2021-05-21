//
// Created by Storm Phoenix on 2021/5/7.
//

#ifndef KAGUYA_METAL_H
#define KAGUYA_METAL_H

#include <kaguya/material/Material.h>
#include <kaguya/material/texture/Texture.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace RENDER_NAMESPACE {
    namespace material {

        using core::Spectrum;
        using texture::Texture;

        class Metal : public Material {
        public:
            Metal(const Texture<Float>::Ptr alpha,
                  const Texture<Spectrum>::Ptr eta,
                  const Texture<Spectrum>::Ptr Ks,
                  const Texture<Spectrum>::Ptr K,
                  std::string distributionType = "ggx");

            virtual void computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                    TransportMode mode = TransportMode::RADIANCE) override;

        private:
            std::string _distributionType;
            const Texture<Float>::Ptr _alpha;
            const Texture<Spectrum>::Ptr _eta;
            const Texture<Spectrum>::Ptr _Ks;
            const Texture<Spectrum>::Ptr _K;
        };
    }
}

#endif //KAGUYA_METAL_H
