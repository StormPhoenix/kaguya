//
// Created by Storm Phoenix on 2021/5/10.
//

#ifndef KAGUYA_PATINAMATERIAL_H
#define KAGUYA_PATINAMATERIAL_H

#include <kaguya/material/Material.h>
#include <kaguya/material/texture/Texture.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace RENDER_NAMESPACE {
    namespace material {
        using namespace texture;

        class PatinaMaterial {
        public:
            // Layered material: Glossy + Diffuse
            PatinaMaterial(const Texture<Spectrum>::Ptr Kd,
                           const Texture<Spectrum>::Ptr Ks,
                           const Texture<Float>::Ptr alpha);

            RENDER_CPU_GPU void evaluateBSDF(SurfaceInteraction &si, MemoryAllocator &allocator,
                                             TransportMode mode = TransportMode::RADIANCE);

            RENDER_CPU_GPU bool isSpecular() {
                return false;
            }

            RENDER_CPU_GPU bool isTwoSided() const {
                return _twoSided;
            }

            RENDER_CPU_GPU void setTwoSided(bool twoSided) {
                _twoSided = twoSided;
            }

        protected:
            bool _twoSided = true;
            const Texture<Spectrum>::Ptr _Kd;
            const Texture<Spectrum>::Ptr _Ks;
            const Texture<Float>::Ptr _alpha;
        };
    }
}

#endif //KAGUYA_PATINAMATERIAL_H
