//
// Created by Storm Phoenix on 2020/11/20.
//

#include <kaguya/core/phase/HenyeyGreensteinFunction.h>

namespace kaguya {
    namespace core {

        HenyeyGreensteinFunction::HenyeyGreensteinFunction(double g) : _g(g) {}

        double HenyeyGreensteinFunction::scatterPdf(const Vector3d &wo, const Vector3d &wi) const {
            return henyeyGreensteinPdf(DOT(wo, wi), _g);
        }

        double HenyeyGreensteinFunction::sampleScatter(const Vector3d &wo, Vector3d *wi,
                                                       Sampler *sampler) const {
            // Henyey-Greenstein phase function
            assert(sampler != nullptr);

            Vector2d sample = sampler->sample2D();
            double sampleU = sample.x;
            double sampleV = sample.y;

            // \cos(\theta) \sin(\theta)
            double term1 = (1 - _g * _g) / (1 + _g * (2 * sampleU - 1));
            double cosTheta;
            if (std::abs(_g) < math::EPSILON) {
                cosTheta = 1 - 2 * sampleU;
            } else {
                cosTheta = (1 + _g * _g - term1 * term1) / (2 * _g);
            }
            double sinTheta = std::sqrt(std::max(0.0, 1 - cosTheta * cosTheta));

            // \phi
            double phi = 2 * math::PI * sampleV;
            double cosPhi = std::cos(phi);
            double sinPhi = std::sin(phi);

            // build local coordinate space
            Vector3d tanY = NORMALIZE(wo);
            Vector3d tanX;
            Vector3d tanZ;
            math::tangentSpace(tanY, &tanX, &tanZ);

            // calculate coordinate
            double y = cosTheta;
            double x = sinTheta * cosPhi;
            double z = sinTheta * sinPhi;

            (*wi) = NORMALIZE(x * tanX + y * tanY + z * tanZ);
            return henyeyGreensteinPdf(cosTheta, _g);
        }

        double HenyeyGreensteinFunction::henyeyGreensteinPdf(double cosTheta, double g) const {
            double denominator = (1 + g * g + 2 * g * cosTheta);
            denominator = denominator * std::sqrt(denominator);
            return math::INV_4PI * (1 - g * g) / denominator;
        }

    }
}