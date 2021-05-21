//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_MATERIAL_H
#define KAGUYA_MATERIAL_H

#include <kaguya/core/Interaction.h>
#include <kaguya/core/bsdf/BSDF.h>
#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/math/Math.h>
#include <kaguya/scene/meta/Shape.h>
#include <kaguya/utils/MemoryArena.h>

namespace RENDER_NAMESPACE {
    namespace material {

        using kaguya::core::Interaction;
        using kaguya::core::SurfaceInteraction;
        using kaguya::core::bsdf::BSDF;
        using kaguya::core::bsdf::BXDF;
        using kaguya::core::bsdf::TransportMode;
        using kaguya::memory::MemoryArena;

        /**
         * 物体材质
         */
        class Material {
        public:

            typedef std::shared_ptr<Material> Ptr;

            /**
             * 计算材质的 bsdf
             * @param insect ray 与 shape 的相交点
             */
            virtual void computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                    TransportMode mode = TransportMode::RADIANCE) = 0;

            virtual bool isSpecular() const {
                return false;
            }

            virtual bool isTwoSided() const {
                return _twoSided;
            }

            virtual void setTwoSided(bool twoSided) {
                _twoSided = twoSided;
            }

        protected:
            bool _twoSided = true;

        };

    }
}

#endif //KAGUYA_MATERIAL_H
