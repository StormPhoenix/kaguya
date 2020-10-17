//
// Created by Storm Phoenix on 2020/10/8.
//

#include <kaguya/core/bsdf/BXDFSpecular.h>
#include <kaguya/math/RefractSampler.h>
#include <kaguya/material/Dielectric.h>

namespace kaguya {
    namespace material {

        using kaguya::math::RefractSampler;
        using kaguya::core::BXDFSpecular;

        Dielectric::Dielectric(std::shared_ptr<Texture> albedo) : Dielectric(albedo, 1.5) {}

        Dielectric::Dielectric(std::shared_ptr<Texture> albedo, double refractiveIndex)
                : _refractiveIndex(refractiveIndex), _albedo(albedo) {
            _pdf = std::make_shared<RefractSampler>(refractiveIndex);
        }

        bool Dielectric::isSpecular() {
            return true;
        }

        bool Dielectric::scatter(const Ray &ray, const Interaction &hitRecord, Ray &scatteredRay, double &pdf) {
            scatteredRay.setOrigin(hitRecord.point);
            if (hitRecord.isFrontFace) {
                // 外表面射入
                scatteredRay.setDirection(_pdf->sample(ray.getDirection(), hitRecord.normal, pdf));
            } else {
                // 内部射入
                scatteredRay.setDirection(_pdf->sample(ray.getDirection(), -hitRecord.normal, pdf));
            }

            return true;
        }

        std::shared_ptr<BSDF> Dielectric::bsdf(kaguya::core::Interaction &insect) {
            Spectrum albedo = _albedo->sample(insect.u, insect.v);
            std::shared_ptr<BXDFSpecular> specularBXDF = std::make_shared<BXDFSpecular>(albedo, 1.0f, _refractiveIndex);
            std::shared_ptr<BSDF> bsdf = std::make_shared<BSDF>(insect);
            bsdf->addBXDF(specularBXDF);
            return bsdf;
        }

        double Dielectric::scatterPDF(const Ray &hitRay, const Interaction &hitRecord, const Ray &scatterRay) {
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