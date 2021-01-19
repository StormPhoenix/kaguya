//
// Created by Storm Phoenix on 2020/11/20.
//

#include <kaguya/core/phase/HenyeyGreensteinFunction.h>

namespace kaguya {
    namespace core {

        HenyeyGreensteinFunction::HenyeyGreensteinFunction(Float g) : _g(g) {}

        Float HenyeyGreensteinFunction::scatterPdf(const Vector3F &wo, const Vector3F &wi) const {
            return henyeyGreensteinPdf(DOT(wo, wi), _g);
        }

        Float HenyeyGreensteinFunction::sampleScatter(const Vector3F &wo, Vector3F *wi,
                                                       Sampler *sampler) const {
            // Henyey-Greenstein phase function
            assert(sampler != nullptr);

            Vector2F sample = sampler->sample2D();
            Float sampleU = sample.x;
            Float sampleV = sample.y;

            // \cos(\theta) \sin(\theta)
            Float term1 = (1 - _g * _g) / (1 + _g * (2 * sampleU - 1));
            Float cosTheta;
            if (std::abs(_g) < math::EPSILON) {
                cosTheta = 1 - 2 * sampleU;
            } else {
                cosTheta = (1 + _g * _g - term1 * term1) / (2 * _g);
            }
            Float sinTheta = std::sqrt(std::max(Float(0.), 1 - cosTheta * cosTheta));

            // \phi
            Float phi = 2 * math::PI * sampleV;
            Float cosPhi = std::cos(phi);
            Float sinPhi = std::sin(phi);

            // build local coordinate space
            Vector3F tanY = NORMALIZE(wo);
            Vector3F tanX;
            Vector3F tanZ;
            math::tangentSpace(tanY, &tanX, &tanZ);

            // calculate coordinate
            Float y = cosTheta;
            Float x = sinTheta * cosPhi;
            Float z = sinTheta * sinPhi;

            (*wi) = NORMALIZE(x * tanX + y * tanY + z * tanZ);
            return henyeyGreensteinPdf(cosTheta, _g);
        }

        Float HenyeyGreensteinFunction::henyeyGreensteinPdf(Float cosTheta, Float g) const {
            Float denominator = (1 + g * g + 2 * g * cosTheta);
            denominator = denominator * std::sqrt(denominator);
            return math::INV_4PI * (1 - g * g) / denominator;
        }

    }
}