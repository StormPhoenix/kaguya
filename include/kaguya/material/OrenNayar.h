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

        class OrenNayar {
        public:
            OrenNayar(Texture<Spectrum>::Ptr Kd, Texture<Float>::Ptr roughness);

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
            Texture<Float>::Ptr _roughness = nullptr;
        };

    }
}

#endif //KAGUYA_ORENNAYAR_H
