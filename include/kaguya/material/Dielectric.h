//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_DIELECTRIC_H
#define KAGUYA_DIELECTRIC_H

#include <kaguya/math/ScatterSampler.h>
#include <kaguya/material/Material.h>
#include <kaguya/material/Texture.h>

namespace kaguya {
    namespace material {

        using kaguya::math::ScatterSampler;

        class Dielectric : public Material {
        public:
            Dielectric(std::shared_ptr<Texture> albedo);

            Dielectric(std::shared_ptr<Texture> albedo, double refractiveIndex);

            virtual bool isSpecular() override;

            virtual bool scatter(const Ray &ray, const Interaction &hitRecord, Ray &scatteredRay, double &pdf) override;

            virtual double scatterPDF(const Ray &hitRay, const Interaction &hitRecord, const Ray &scatterRay) override;

            virtual std::shared_ptr<BSDF> bsdf(Interaction &insect) override;


        private:
            double _refractiveIndex;

            std::shared_ptr<Texture> _albedo = nullptr;

            std::shared_ptr<ScatterSampler> _pdf = nullptr;
        };

    }
}

#endif //KAGUYA_DIELECTRIC_H
