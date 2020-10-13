//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/math/HemiCosineSampler.h>
#include <kaguya/math/HemiUniformSampler.h>
#include <kaguya/material/Lambertian.h>

namespace kaguya {
    namespace material {

        using kaguya::math::HemiCosineSampler;
        using kaguya::math::HemiUniformSampler;

        Lambertian::Lambertian(std::shared_ptr<Texture> albedo) : _albedo(albedo) {
            _pdf = std::make_shared<HemiCosineSampler>();
//            _pdf = std::make_shared<HemiUniformSampler>();
        }

        double Lambertian::scatterPDF(const Ray &hitRay, const Interaction &hitRecord, const Ray &scatterRay) {
            return _pdf->pdf(hitRay.getDirection(), hitRecord.normal, scatterRay.getDirection());
        }

        bool Lambertian::scatter(const Ray &ray, const Interaction &hitRecord, Ray &scatteredRay, double &pdf) {
            scatteredRay.setOrigin(hitRecord.point);
            scatteredRay.setDirection(_pdf->sample(ray.getDirection(), hitRecord.normal, pdf));
            return true;
        }

        Vector3 Lambertian::brdf(const Interaction &hitRecord, const Vector3 &scatterDirection) {
            return _albedo->sample(hitRecord.u, hitRecord.v);
        }

    }
}