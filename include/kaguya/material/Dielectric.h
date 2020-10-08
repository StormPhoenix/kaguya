//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_DIELECTRIC_H
#define KAGUYA_DIELECTRIC_H

#include <kaguya/math/ScatterPdf.h>
#include <kaguya/material/Material.h>
#include <kaguya/material/Texture.h>

namespace kaguya {
    namespace material {

        using kaguya::math::ScatterPdf;

        class Dielectric : public Material {
        public:
            Dielectric(std::shared_ptr<Texture> albedo);

            Dielectric(std::shared_ptr<Texture> albedo, double refractiveIndex);

            virtual bool scatter(const Ray &ray, const HitRecord &hitRecord, Ray &scatteredRay, float &pdf) override;

            virtual Vector3 brdf(const HitRecord &hitRecord, const Vector3 &scatterDirection) override;

            virtual double scatterPDF(const Ray &hitRay, const HitRecord &hitRecord, const Ray &scatterRay) override;

        private:
            double _refractiveIndex;

            std::shared_ptr<Texture> _albedo = nullptr;

            std::shared_ptr<ScatterPdf> _pdf = nullptr;
        };

    }
}

#endif //KAGUYA_DIELECTRIC_H
