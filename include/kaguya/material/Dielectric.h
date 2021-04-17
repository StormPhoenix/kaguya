//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_DIELECTRIC_H
#define KAGUYA_DIELECTRIC_H

#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/material/Material.h>
#include <kaguya/material/texture/Texture.h>

namespace kaguya {
    namespace material {

        using namespace texture;
        using namespace core;

        class Dielectric : public Material {
        public:
            Dielectric(std::shared_ptr<Texture<Spectrum>> albedo);

            Dielectric(std::shared_ptr<Texture<Spectrum>> albedo, Float refractiveIndex);

            Dielectric(std::shared_ptr<Texture<Spectrum>> albedo, Float thetaI, Float thetaT);

            virtual bool isSpecular() const override;

            virtual void computeScatteringFunctions(SurfaceInteraction &insect, MemoryArena &memoryArena,
                                                    TransportMode mode = TransportMode::RADIANCE) override;


        private:
            Float _thetaI, _thetaT;
            std::shared_ptr<Texture<Spectrum>> _albedo = nullptr;
        };

    }
}

#endif //KAGUYA_DIELECTRIC_H
