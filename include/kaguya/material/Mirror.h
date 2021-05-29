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

        class Mirror : public Material {
        public:
            Mirror();

            Mirror(std::shared_ptr<Texture<Spectrum>> albedo, Float fuzzy = 0);

            virtual bool isSpecular() const override;

            virtual void computeScatteringFunctions(SurfaceInteraction &insect, MemoryAllocator &allocator,
                                                    TransportMode mode = TransportMode::RADIANCE) override;

        private:
            // 反射率
            std::shared_ptr<Texture<Spectrum>> _albedo;
            // 毛玻璃效果系数
            Float _fuzzy;
        };

    }
}

#endif //KAGUYA_MIRROR_H
