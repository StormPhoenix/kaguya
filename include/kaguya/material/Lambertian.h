//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_LAMBERTIAN_H
#define KAGUYA_LAMBERTIAN_H

#include <kaguya/math/ScatterPdf.h>

#include <kaguya/material/Material.h>
#include <kaguya/material/Texture.h>
#include <memory>

namespace kaguya {
    namespace material {

        using kaguya::math::ScatterPdf;

        class Lambertian : public Material {
        public:
            Lambertian(std::shared_ptr<Texture> albedo);

            double scatterPDF(const Ray &hitRay, const HitRecord &hitRecord, const Ray &scatterRay) override;

            bool scatter(const Ray &ray, const HitRecord &hitRecord, Ray &scatteredRay, double &pdf) override;

            Vector3 brdf(const HitRecord &hitRecord, const Vector3 &scatterDirection) override;

        private:
            std::shared_ptr<Texture> _albedo = nullptr;

            std::shared_ptr<ScatterPdf> _pdf = nullptr;
        };

    }
}

#endif //KAGUYA_LAMBERTIAN_H
