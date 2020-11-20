//
// Created by Storm Phoenix on 2020/11/20.
//

#include <kaguya/core/phase/HenyeyGreensteinFunction.h>

namespace kaguya {
    namespace core {

        double HenyeyGreensteinFunction::scatterPdf(const Vector3 &wo, const Vector3 &wi) const {
            return henyeyGreensteinPdf(DOT(wo, wi), _g);
        }

        double HenyeyGreensteinFunction::henyeyGreensteinPdf(double cosTheta, double g) const {
            double denominator = (1 + g * g + 2 * g * cosTheta);
            denominator = denominator * std::sqrt(denominator);
            return INV_4PI * (1 - g * g) / denominator;
        }

    }
}