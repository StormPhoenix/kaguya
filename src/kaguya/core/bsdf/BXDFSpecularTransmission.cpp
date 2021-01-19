//
// Created by Storm Phoenix on 2020/10/15.
//

#include <kaguya/core/bsdf/BXDFSpecularTransmission.h>
#include <kaguya/core/bsdf/FresnelDielectric.h>

namespace kaguya {
    namespace core {

        BXDFSpecularTransmission::BXDFSpecularTransmission(const Spectrum &albedo, Float thetaI, Float thetaT,
                                                           FresnelDielectric *fresnel, TransportMode mode) :
                BXDF(BXDFType(BSDF_SPECULAR | BSDF_TRANSMISSION)), _thetaI(thetaI), _thetaT(thetaT), _mode(mode) {
            _fresnel = fresnel;
        }

        Spectrum BXDFSpecularTransmission::f(const Vector3F &wo, const Vector3F &wi) const {
            return Spectrum(0.0f);
        }

        Spectrum BXDFSpecularTransmission::sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                                   Sampler *const sampler) {
            Float refraction;
            if (wo.y > 0) {
                // 外部射入
                refraction = _thetaI / _thetaT;
            } else {
                // 内部射入
                refraction = _thetaT / _thetaI;
            }

            Vector3F normal = Vector3F(0.0, 1.0, 0.0);
            if (DOT(wo, normal) < 0) {
                normal.y = -1;
            }

            if (!math::refract(wo, normal, refraction, wi)) {
                // 发射了全反射，无法折射
                return 0;
            }
            *pdf = 1;
            Float cosineThetaT = abs(wi->y);
            Spectrum f = _albedo * (Spectrum(1.0) - _fresnel->fresnel(cosineThetaT)) / cosineThetaT;
            if (_mode == TransportMode::RADIANCE) {
                f *= std::pow(refraction, 2);
            }
            return f;
        }

        Float BXDFSpecularTransmission::samplePdf(const Vector3F &wo, const Vector3F &wi) const {
            return 0.0;
        }
    }
}