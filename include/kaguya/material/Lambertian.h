//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_LAMBERTIAN_H
#define KAGUYA_LAMBERTIAN_H

#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/material/Material.h>
#include <kaguya/material/texture/Texture.h>
#include <memory>

namespace kaguya {
    namespace material {

        using namespace texture;
        using namespace core;

        class Lambertian : public Material {
        public:

            Lambertian(std::shared_ptr<Texture<Spectrum>> albedo);

            virtual void computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                     TransportMode mode = TransportMode::RADIANCE) override;

        private:

//            template<int Channel>
            std::shared_ptr<Texture<Spectrum>> _albedo = nullptr;
        };

    }
}

#endif //KAGUYA_LAMBERTIAN_H
