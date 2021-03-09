//
// Created by Storm Phoenix on 2020/10/15.
//

#include <kaguya/core/bsdf/BXDF.h>

namespace kaguya {
    namespace core {

        BXDF::BXDF(BXDFType type) : type(type) {}

        Spectrum BXDF::sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                               Sampler *const sampler, BXDFType *sampleType) {
            // 半球面采样
            *wi = math::sampling::hemiCosineSampling(sampler);
            if (wo.y < 0) {
                wi->y *= -1;
            }
            // 采样 surfacePointPdf
            *pdf = samplePdf(wo, *wi);
            return f(wo, *wi);
        }

        Float BXDF::samplePdf(const Vector3F &wo, const Vector3F &wi) const {
            if (wo.y * wi.y > 0) {
                return std::abs(wi.y) * math::INV_PI;
            } else {
                return 0;
            }
        }

        bool BXDF::allIncludeOf(const BXDFType bxdfType) {
            return (type & bxdfType) == type;
        }

        bool BXDF::hasAllOf(const BXDFType bxdfType) {
            return (type & bxdfType) == bxdfType;
        }

        bool BXDF::hasAnyOf(const BXDFType bxdfType) {
            return (type & bxdfType) > 0;
        }
    }
}