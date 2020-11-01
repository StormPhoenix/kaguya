//
// Created by Storm Phoenix on 2020/10/29.
//

#include <kaguya/core/Interaction.h>
#include <kaguya/material/Material.h>

namespace kaguya {
    namespace core {

        using kaguya::material::Material;

        BSDF *SurfaceInteraction::buildBSDF(MemoryArena &memoryArena, TransportMode mode) {
            assert(material != nullptr);
            bsdf = material->bsdf(*this, memoryArena, mode);
            return bsdf;
        }

    }
}