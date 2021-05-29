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

        class PatinaMaterial : public Material {
        public:
            // Layered material: Glossy + Diffuse
            PatinaMaterial(const Texture<Spectrum>::Ptr Kd,
                           const Texture<Spectrum>::Ptr Ks,
                           const Texture<Float>::Ptr alpha);

            virtual void computeScatteringFunctions(SurfaceInteraction &insect, MemoryAllocator &allocator,
                                                    TransportMode mode = TransportMode::RADIANCE) override;

        protected:
            const Texture<Spectrum>::Ptr _Kd;
            const Texture<Spectrum>::Ptr _Ks;
            const Texture<Float>::Ptr _alpha;
        };
    }
}

#endif //KAGUYA_PATINAMATERIAL_H
