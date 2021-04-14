//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_LAMBERTIAN_H
#define KAGUYA_LAMBERTIAN_H

#include <kaguya/material/Material.h>
#include <kaguya/material/texture/Texture.h>
#include <memory>

namespace kaguya {
    namespace material {

        class Lambertian : public Material {
        public:
            Lambertian(std::shared_ptr<Texture> albedo);

            virtual void computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                     TransportMode mode = TransportMode::RADIANCE) override;

        private:
            std::shared_ptr<Texture> _albedo = nullptr;
        };

    }
}

#endif //KAGUYA_LAMBERTIAN_H
