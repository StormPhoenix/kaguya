//
// Created by Storm Phoenix on 2020/10/15.
//

#include <kaguya/core/bsdf/BXDFSpecularTransmission.h>
#include <kaguya/core/bsdf/FresnelDielectric.h>

namespace kaguya {
    namespace core {

        BXDFSpecularTransmission::BXDFSpecularTransmission(const Spectrum &albedo, double thetaI, double thetaT,
                                                           std::shared_ptr<Fresnel> fresnel) :
                BXDF(BXDFType(BSDF_SPECULAR | BSDF_TRANSMISSION)), _thetaI(thetaI), _thetaT(thetaT) {
            _fresnel = std::make_shared<FresnelDielectric>(thetaI, thetaT);
        }

        Spectrum BXDFSpecularTransmission::f(const Vector3 &wo, const Vector3 &wi) const {
            return Spectrum(0.0f);
        }

        Spectrum BXDFSpecularTransmission::sampleF(const Vector3 &wo, Vector3 *wi, double *pdf) {
            double refraction;
            if (wo.y > 0) {
                // 外部射入
                refraction = _thetaI / _thetaT;
            } else {
                // 内部射入
                refraction = _thetaT / _thetaI;
            }

            Vector3 normal = Vector3(0.0, 1.0, 0.0);
            if (DOT(wo, normal) < 0) {
                normal.y = -1;
            }

            if (!refract(wo, normal, refraction, wi)) {
                // 发射了全反射，无法折射
                return 0;
            }
            *pdf = 1;
            double cosineThetaI = abs(wo.y);
            double cosineThetaT = abs(wi->y);
            return _albedo * (Spectrum(1.0) - _fresnel->fresnel(cosineThetaI)) / cosineThetaT;
        }

        double BXDFSpecularTransmission::samplePdf(const Vector3 &wo, const Vector3 &wi) const {
            return 0.0;
        }
    }
}