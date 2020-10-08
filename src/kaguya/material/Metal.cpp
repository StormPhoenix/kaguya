//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/material/Metal.h>
#include <kaguya/math/HemiReflectPdf.h>

namespace kaguya {
    namespace material {

        using kaguya::math::HemiReflectPdf;

        Metal::Metal() : Metal(Vector3(1.0f, 1.0f, 1.0f), 0) {}

        Metal::Metal(const Vector3 &albedo, double fuzzy) : _albedo(albedo), _fuzzy(fuzzy) {
            // TODO 判断 _fuzzy
            _pdf = std::make_shared<HemiReflectPdf>();
        }

        bool Metal::scatter(const Ray &ray, const HitRecord &hitRecord, Ray &scatteredRay, float &pdf) {
            double cosine = DOT(NORMALIZE(ray.getDirection()), NORMALIZE(hitRecord.normal));
            if (cosine < 0) {
                // 从物体外部击中
                scatteredRay.setOrigin(hitRecord.point);
                scatteredRay.setDirection(_pdf->random(ray.getDirection(), hitRecord.normal, pdf));
                return true;
            } else {
                // 从物体内部击中，将在内部反射
                scatteredRay.setOrigin(hitRecord.point);
                scatteredRay.setDirection(_pdf->random(ray.getDirection(), -hitRecord.normal, pdf));
                return true;
            }
        }

        Vector3 Metal::brdf(const HitRecord &hitRecord, const Vector3 &scatterDirection) {
            return _albedo;
        }

        double Metal::scatterPDF(const Ray &hitRay, const HitRecord &hitRecord, const Ray &scatterRay) {
            double cosine = DOT(NORMALIZE(hitRay.getDirection()), NORMALIZE(hitRecord.normal));
            if (cosine < 0) {
                // 从物体外部击中
                return _pdf->pdf(hitRay.getDirection(), hitRecord.normal, scatterRay.getDirection());
            } else {
                // 从物体内部击中，将在内部反射
                return _pdf->pdf(hitRay.getDirection(), -hitRecord.normal, scatterRay.getDirection());
            }
        }
    }
}