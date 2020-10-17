//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/core/bsdf/BXDFSpecularReflection.h>
#include <kaguya/core/bsdf/FresnelDefault.h>
#include <kaguya/material/Metal.h>
#include <kaguya/math/HemiReflectSampler.h>

namespace kaguya {
    namespace material {

        using kaguya::math::HemiReflectSampler;
        using kaguya::core::BXDFSpecularReflection;
        using kaguya::core::FresnelDefault;

        Metal::Metal() : Metal(Spectrum(1.0f), 0) {}

        Metal::Metal(const Spectrum &albedo, double fuzzy) : _albedo(albedo), _fuzzy(fuzzy) {
            // TODO 判断 _fuzzy
            // TODO delete
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

        double Metal::scatterPDF(const Ray &hitRay, const Interaction &hitRecord, const Ray &scatterRay) {
            return _pdf->pdf(hitRay.getDirection(), hitRecord.normal, scatterRay.getDirection());
        }

        std::shared_ptr<BSDF> Metal::bsdf(Interaction &insect) {
            std::shared_ptr<FresnelDefault> fresnel = std::make_shared<FresnelDefault>();
            std::shared_ptr<BXDFSpecularReflection> bxdf = std::make_shared<BXDFSpecularReflection>(_albedo, fresnel);

            std::shared_ptr<BSDF> bsdf = std::make_shared<BSDF>(insect);
            bsdf->addBXDF(bxdf);
            return bsdf;
        }
    }
}