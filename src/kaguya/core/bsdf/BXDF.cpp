//
// Created by Storm Phoenix on 2020/10/15.
//

#include <kaguya/core/bsdf/BXDF.h>

namespace kaguya {
    namespace core {

        BXDF::BXDF(BXDFType type) : type(type) {}

        Spectrum BXDF::sampleF(const Vector3 &wo, Vector3 *wi, double *pdf) {
            // 半球面采样
            *wi = hemiCosineSampling();
            if (wo.y < 0) {
                wi->y *= -1;
            }
            // 采样 pdf
            *pdf = samplePdf(wo, *wi);
            return f(wo, *wi);
        }

        double BXDF::samplePdf(const Vector3 &wo, const Vector3 &wi) const {
            if (wo.y * wi.y > 0) {
                return std::abs(wi.y) * INV_PI;
            } else {
                return 0;
            }
        }

        bool BXDF::belongToType(const BXDFType bxdfType) {
            return (type & bxdfType) == type;
        }

        bool BXDF::containType(const BXDFType bxdfType) {
            return (type & bxdfType) == bxdfType;
        }

        Vector3 BXDF::hemiCosineSampling() {
            // fi = 2 * Pi * sampleU
            double sampleU = uniformSample();
            // sampleV = sin^2(theta)
            double sampleV = uniformSample();
            // x = sin(theta) * cos(fi)
            double x = sqrt(sampleV) * cos(2 * PI * sampleU);
            // y = cos(theta)
            double y = sqrt(1 - sampleV);
            // z = sin(theta) * sin(fi)
            double z = sqrt(sampleV) * sin(2 * PI * sampleU);
            return NORMALIZE(Vector3(x, y, z));
        }

    }
}