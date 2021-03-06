//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_DIELECTRIC_H
#define KAGUYA_DIELECTRIC_H

#include <kaguya/material/Material.h>
#include <kaguya/material/Texture.h>

namespace kaguya {
    namespace material {

        class Dielectric : public Material {
        public:
            Dielectric(std::shared_ptr<Texture> albedo);

            Dielectric(std::shared_ptr<Texture> albedo, Float refractiveIndex);

            virtual bool isSpecular() const override;

            virtual void computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                     TransportMode mode = TransportMode::RADIANCE) override;


        private:
            Float _refractiveIndex;

            std::shared_ptr<Texture> _albedo = nullptr;
        };

    }
}

#endif //KAGUYA_DIELECTRIC_H
