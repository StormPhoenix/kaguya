//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/material/Metal.h>
#include <kaguya/math/HemiReflectSampler.h>

namespace kaguya {
    namespace material {

        using kaguya::math::HemiReflectSampler;

        Metal::Metal() : Metal(Vector3(1.0f, 1.0f, 1.0f), 0) {}

        Metal::Metal(const Vector3 &albedo, double fuzzy) : _albedo(albedo), _fuzzy(fuzzy) {
            // TODO 判断 _fuzzy
            _pdf = std::make_shared<HemiReflectSampler>();
        }

        bool Metal::isSpecular() {
            return true;
        }

        bool Metal::scatter(const Ray &ray, const Interaction &hitRecord, Ray &scatteredRay, double &pdf) {
            scatteredRay.setOrigin(hitRecord.point);
            scatteredRay.setDirection(_pdf->sample(ray.getDirection(), hitRecord.normal, pdf));
            return true;
        }

        Vector3 Metal::brdf(const Interaction &hitRecord, const Vector3 &scatterDirection) {
            return _albedo;
        }

        double Metal::scatterPDF(const Ray &hitRay, const Interaction &hitRecord, const Ray &scatterRay) {
            return _pdf->pdf(hitRay.getDirection(), hitRecord.normal, scatterRay.getDirection());
        }
    }
}