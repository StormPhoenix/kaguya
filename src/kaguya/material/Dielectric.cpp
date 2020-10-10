//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/math/RefractPdf.h>
#include <kaguya/material/Dielectric.h>

namespace kaguya {
    namespace material {

        using kaguya::math::RefractPdf;

        Dielectric::Dielectric(std::shared_ptr<Texture> albedo) : Dielectric(albedo, 1.5) {}

        Dielectric::Dielectric(std::shared_ptr<Texture> albedo, double refractiveIndex)
                : _refractiveIndex(refractiveIndex), _albedo(albedo) {
            _pdf = std::make_shared<RefractPdf>(refractiveIndex);
        }

        bool Dielectric::isSpecular() {
            return true;
        }

        bool Dielectric::scatter(const Ray &ray, const HitRecord &hitRecord, Ray &scatteredRay, double &pdf) {
            scatteredRay.setOrigin(hitRecord.point);
            if (hitRecord.isFrontFace) {
                // 外表面射入
                scatteredRay.setDirection(_pdf->random(ray.getDirection(), hitRecord.normal, pdf));
            } else {
                // 内部射入
                scatteredRay.setDirection(_pdf->random(ray.getDirection(), -hitRecord.normal, pdf));
            }

            return true;
        }

        Vector3 Dielectric::brdf(const HitRecord &hitRecord, const Vector3 &scatterDirection) {
            return _albedo->sample(hitRecord.u, hitRecord.v);
        }

        double Dielectric::scatterPDF(const Ray &hitRay, const HitRecord &hitRecord, const Ray &scatterRay) {
            if (hitRecord.isFrontFace) {
                // 外表面射入
                return _pdf->pdf(hitRay.getDirection(), hitRecord.normal, scatterRay.getDirection());
            } else {
                // 内部射入
                return _pdf->pdf(hitRay.getDirection(), -hitRecord.normal, scatterRay.getDirection());
            }
        }
    }
}