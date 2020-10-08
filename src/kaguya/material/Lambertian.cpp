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
        }

        double Lambertian::scatterPDF(const Ray &hitRay, const HitRecord &hitRecord, const Ray &scatterRay) {
            double cosine = DOT(NORMALIZE(hitRay.getDirection()), NORMALIZE(hitRecord.normal));
            if (cosine < 0) {
                // 从物体外部击中
                return _pdf->pdf(hitRay.getDirection(), hitRecord.normal, scatterRay.getDirection());
            } else {
                // 从物体内部击中
                return _pdf->pdf(hitRay.getDirection(), -hitRecord.normal, scatterRay.getDirection());
            }
        }

        bool Lambertian::scatter(const Ray &ray, const HitRecord &hitRecord, Ray &scatteredRay, float &pdf) {
            double cosine = DOT(NORMALIZE(ray.getDirection()), NORMALIZE(hitRecord.normal));
            if (cosine < 0) {
                // 从物体外部击中
                scatteredRay.setOrigin(hitRecord.point);
                scatteredRay.setDirection(_pdf->random(ray.getDirection(), hitRecord.normal, pdf));
                return true;
            } else {
                // 从物体内部击中
                scatteredRay.setOrigin(hitRecord.point);
                scatteredRay.setDirection(_pdf->random(ray.getDirection(), -hitRecord.normal, pdf));
                return true;
            }
        }

        Vector3 Lambertian::brdf(const HitRecord &hitRecord, const Vector3 &scatterDirection) {
            return _albedo->sample(hitRecord.u, hitRecord.v);
        }

    }
}