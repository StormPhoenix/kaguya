//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_LAMBERTIAN_H
#define KAGUYA_LAMBERTIAN_H

#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/material/Material.h>
#include <kaguya/material/texture/Texture.h>
#include <memory>

namespace RENDER_NAMESPACE {
    namespace material {

        using namespace texture;
        using namespace core;

        class Lambertian {
        public:
            Lambertian(std::shared_ptr<Texture<Spectrum>> Kd);

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
            std::shared_ptr<Texture<Spectrum>> _Kd = nullptr;
        };

    }
}

#endif //KAGUYA_LAMBERTIAN_H
