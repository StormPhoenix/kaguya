//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_METAL_H
#define KAGUYA_METAL_H

#include <kaguya/core/spectrum/Spectrum.hpp>

#include <kaguya/material/Material.h>
#include <kaguya/math/Math.hpp>
#include <kaguya/math/ScatterSampler.h>

namespace kaguya {
    namespace material {

        using kaguya::core::Spectrum;
        using kaguya::math::ScatterSampler;

        class Metal : public Material {
        public:
            Metal();

            Metal(const Spectrum &albedo, double fuzzy = 0);

            virtual bool isSpecular() override;

            virtual BSDF *bsdf(SurfaceInteraction &insect, MemoryArena &memoryArena) override;

        private:
            // 反射率
            Spectrum _albedo;
            // 毛玻璃效果系数
            double _fuzzy;
        };

    }
}

#endif //KAGUYA_METAL_H
