//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_MATERIAL_H
#define KAGUYA_MATERIAL_H

#include <kaguya/core/Interaction.h>
#include <kaguya/core/bsdf/BSDF.h>
#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/math/Math.hpp>
#include <kaguya/scene/meta/Shape.h>
#include <kaguya/utils/MemoryArena.h>

namespace kaguya {
    namespace material {

        using kaguya::core::Interaction;
        using kaguya::core::SurfaceInteraction;
        using kaguya::core::BSDF;
        using kaguya::core::BXDF;
        using kaguya::core::TransportMode;
        using kaguya::memory::MemoryArena;

        /**
         * 物体材质
         */
        class Material {
        public:
            /**
             * 计算材质的 bsdf
             * @param insect ray 与 shape 的相交点
             */
            virtual BSDF *bsdf(SurfaceInteraction &insect, MemoryArena &memoryArena,
                               TransportMode mode = TransportMode::RADIANCE) = 0;

            /**
             * TODO delete 逐渐删除 isSpecular
             * 是否具有反光、折射属性
             * @return
             */
            virtual bool isSpecular() const {
                return false;
            }

        };

    }
}

#endif //KAGUYA_MATERIAL_H
