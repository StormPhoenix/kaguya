//
// Created by Storm Phoenix on 2021/5/7.
//

#ifndef KAGUYA_BXDFORENNAYAR_CPP
#define KAGUYA_BXDFORENNAYAR_CPP

#include <kaguya/core/bsdf/BXDFOrenNayar.h>
#include <kaguya/math/Math.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            BXDFOrenNayar::BXDFOrenNayar(const Spectrum &reflectance, Float roughness) :
                    BXDF(BXDFType(BXDFType::BSDF_REFLECTION | BXDFType::BSDF_DIFFUSE)), _Kd(reflectance) {
                A = 1 - (roughness * roughness / (2 * (roughness * roughness + 0.33)));
                B = 0.45 * roughness * roughness / (roughness * roughness + 0.09);
            }

            Spectrum BXDFOrenNayar::f(const Vector3F &wo, const Vector3F &wi) const {
                if (wo.y * wi.y <= 0) {
                    return Spectrum(0.0f);
                }

                Float sinThetaWo = math::local_coord::sinTheta(wo);
                Float sinThetaWi = math::local_coord::sinTheta(wi);
                Float sinAlpha = std::max(std::abs(sinThetaWo), std::abs(sinThetaWi));

                Float tanThetaWo = math::local_coord::tanTheta(wo);
                Float tanThetaWi = math::local_coord::tanTheta(wi);
                Float tanBeta = std::min(std::abs(tanThetaWo), std::abs(tanThetaWi));

                Vector2F phi_Wi = Vector2F(wi.x, wi.z);
                Vector2F phi_Wo = Vector2F(wo.x, wo.z);

                Float cosPhi = DOT(phi_Wi, phi_Wo);
                return _Kd / math::PI * (A + B * std::max(Float(0), cosPhi) * sinAlpha * tanBeta);
            }
        }
    }
}
#endif //KAGUYA_BXDFORENNAYAR_CPP
