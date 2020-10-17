//
// Created by Storm Phoenix on 2020/10/15.
//

#include <kaguya/core/bsdf/BXDFSpecularReflection.h>

namespace kaguya {
    namespace core {

        BXDFSpecularReflection::BXDFSpecularReflection(const Spectrum &albedo, std::shared_ptr<Fresnel> fresnel) :
                BXDF(BXDFType(BSDF_SPECULAR | BSDF_REFLECTION)),
                _albedo(albedo),
                _fresnel(fresnel) {

        }

        Spectrum BXDFSpecularReflection::f(const Vector3 &wo, const Vector3 &wi) const {
            return Spectrum(0.0f);
        }

        Spectrum BXDFSpecularReflection::sampleF(const Vector3 &wo, Vector3 *wi, double *pdf) {
            assert(_fresnel != nullptr);
            *wi = Vector3(-wo.x, wo.y, -wo.z);
            *pdf = 1;
            // 除以 cosine(w_i) 是为了不让能量损耗，这个可以利用 hemisphere-directional 积分得到
            return _fresnel->fresnel(wo.y) * _albedo / std::abs((*wi).y);
        }

        double BXDFSpecularReflection::samplePdf(const Vector3 &wo, const Vector3 &wi) const {
            return 0;
        }
    }
}