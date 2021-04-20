//
// Created by Storm Phoenix on 2020/10/15.
//

#include <kaguya/core/bsdf/BXDFLambertianReflection.h>

namespace kaguya {
    namespace core {

        BXDFLambertianReflection::BXDFLambertianReflection(const Spectrum &albedo) :
                BXDF(BXDFType(BSDF_DIFFUSE | BSDF_REFLECTION)), _albedo(albedo) {}

        Spectrum BXDFLambertianReflection::f(const Vector3F &wo, const Vector3F &wi) const {
            /**
             * Lambertian 的吸收率是 _albedo，这个值等于半球面所有方向的入射辐射超某一个方向辐射出的反射率，
             * 我们假设 reflectance = C 是一个常量，带入积分方程可以解出 C = albedo / PI
             */
             return _albedo * math::INV_PI;
        }

    }
}