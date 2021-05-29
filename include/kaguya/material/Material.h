//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_MATERIAL_H
#define KAGUYA_MATERIAL_H

#include <kaguya/common.h>
#include <kaguya/core/core.h>
#include <kaguya/core/Interaction.h>
#include <kaguya/utils/TaggedPointer.h>
#include <kaguya/utils/memory/MemoryAllocator.h>

namespace RENDER_NAMESPACE {
    namespace material {

        using memory::TaggedPointer;
        using kaguya::core::Interaction;
        using kaguya::core::SurfaceInteraction;
        using kaguya::core::bsdf::TransportMode;
        using kaguya::memory::MemoryAllocator;

        class Dielectric;

        class Lambertian;

        class Metal;

        class Mirror;

        class OrenNayar;

        class PatinaMaterial;

        class PlasticMaterial;

        class SubsurfaceMaterial;

        class Material : public TaggedPointer<Dielectric, Lambertian, Metal, Mirror,
                OrenNayar, PatinaMaterial, PlasticMaterial, SubsurfaceMaterial> {
        public:
            using TaggedPointer::TaggedPointer;

            RENDER_CPU_GPU void evaluateBSDF(SurfaceInteraction &insect, MemoryAllocator &allocator,
                                             TransportMode mode = TransportMode::RADIANCE);

            RENDER_CPU_GPU bool isSpecular();

            RENDER_CPU_GPU bool isTwoSided();

            RENDER_CPU_GPU void setTwoSided(bool twoSided);
        };

    }
}

#endif //KAGUYA_MATERIAL_H
