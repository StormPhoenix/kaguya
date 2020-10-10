//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/math/HemiCosinePdf.h>
#include <kaguya/math/HemiUniformPdf.h>
#include <kaguya/material/Lambertian.h>

namespace kaguya {
    namespace material {

        using kaguya::math::HemiCosinePdf;
        using kaguya::math::HemiUniformPdf;

        Lambertian::Lambertian(std::shared_ptr<Texture> albedo) : _albedo(albedo) {
            _pdf = std::make_shared<HemiCosinePdf>();
//            _pdf = std::make_shared<HemiUniformPdf>();
        }

        double Lambertian::scatterPDF(const Ray &hitRay, const HitRecord &hitRecord, const Ray &scatterRay) {
            return _pdf->pdf(hitRay.getDirection(), hitRecord.normal, scatterRay.getDirection());
        }

        bool Lambertian::scatter(const Ray &ray, const HitRecord &hitRecord, Ray &scatteredRay, double &pdf) {
            scatteredRay.setOrigin(hitRecord.point);
            scatteredRay.setDirection(_pdf->random(ray.getDirection(), hitRecord.normal, pdf));
            return true;
        }

        Vector3 Lambertian::brdf(const HitRecord &hitRecord, const Vector3 &scatterDirection) {
            return _albedo->sample(hitRecord.u, hitRecord.v);
        }

    }
}