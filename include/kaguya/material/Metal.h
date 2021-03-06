//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_METAL_H
#define KAGUYA_METAL_H

#include <kaguya/core/spectrum/Spectrum.hpp>

#include <kaguya/material/Material.h>
#include <kaguya/math/Math.h>

namespace kaguya {
    namespace material {

        using kaguya::core::Spectrum;

        class Metal : public Material {
        public:
            Metal();

            Metal(const Spectrum &albedo, Float fuzzy = 0);

            virtual bool isSpecular() const override;

            virtual void computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                     TransportMode mode = TransportMode::RADIANCE) override;

        private:
            // 反射率
            Spectrum _albedo;
            // 毛玻璃效果系数
            Float _fuzzy;
        };

    }
}

#endif //KAGUYA_METAL_H
