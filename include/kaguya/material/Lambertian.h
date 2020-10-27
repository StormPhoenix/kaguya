//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_LAMBERTIAN_H
#define KAGUYA_LAMBERTIAN_H

#include <kaguya/math/ScatterSampler.h>

#include <kaguya/material/Material.h>
#include <kaguya/material/Texture.h>
#include <memory>

namespace kaguya {
    namespace material {

        using kaguya::math::ScatterSampler;

        class Lambertian : public Material {
        public:
            Lambertian(std::shared_ptr<Texture> albedo);

            virtual BSDF *bsdf(SurfaceInteraction &insect, MemoryArena &memoryArena) override;

        private:
            std::shared_ptr<Texture> _albedo = nullptr;

            std::shared_ptr<ScatterSampler> _pdf = nullptr;
        };

    }
}

#endif //KAGUYA_LAMBERTIAN_H
