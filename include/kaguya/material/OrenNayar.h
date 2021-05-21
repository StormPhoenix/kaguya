//
// Created by Storm Phoenix on 2021/5/12.
//

#ifndef KAGUYA_ORENNAYAR_H
#define KAGUYA_ORENNAYAR_H

#include <kaguya/material/Material.h>
#include <kaguya/material/texture/Texture.h>

namespace RENDER_NAMESPACE {
    namespace material {

        using namespace texture;
//        using namespace core;

        class OrenNayar : public Material {
        public:
            OrenNayar(Texture<Spectrum>::Ptr Kd, Texture<Float>::Ptr roughness);

            virtual void computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                    TransportMode mode = TransportMode::RADIANCE) override;

        private:
            Texture<Spectrum>::Ptr _Kd = nullptr;
            Texture<Float>::Ptr _roughness = nullptr;
        };

    }
}

#endif //KAGUYA_ORENNAYAR_H
