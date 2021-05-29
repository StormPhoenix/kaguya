//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_MIRROR_H
#define KAGUYA_MIRROR_H

#include <kaguya/math/Math.h>
#include <kaguya/material/Material.h>
#include <kaguya/material/texture/Texture.h>

namespace RENDER_NAMESPACE {
    namespace material {
        using namespace texture;
        using kaguya::core::Spectrum;

        class Mirror {
        public:
            Mirror();

            Mirror(std::shared_ptr<Texture<Spectrum>> albedo);

            RENDER_CPU_GPU void evaluateBSDF(SurfaceInteraction &si, MemoryAllocator &allocator,
                                             TransportMode mode = TransportMode::RADIANCE);

            RENDER_CPU_GPU bool isSpecular() {
                return true;
            }

            RENDER_CPU_GPU bool isTwoSided() {
                return _twoSided;
            }

            RENDER_CPU_GPU void setTwoSided(bool twoSided) {
                _twoSided = twoSided;
            }

        private:
            bool _twoSided = true;
            std::shared_ptr<Texture<Spectrum>> _albedo;
        };

    }
}

#endif //KAGUYA_MIRROR_H
