//
// Created by Storm Phoenix on 2021/5/12.
//

#include <kaguya/math/Math.h>
#include <kaguya/core/bsdf/microfacet/GGXDistribution.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            namespace microfacet {
                using namespace math;

                GGXDistribution::GGXDistribution(Float alpha_g) : _alpha_g(alpha_g) {
                    ASSERT(_alpha_g > 0, "GGXDistribution parameter error: alpha_g lower than zero. ");
                }

                Float GGXDistribution::D(const Normal3F &wh) const {
                    Float cosThetaH = local_coord::cosTheta(wh);
                    if (cosThetaH <= 0) {
                        return 0;
                    }

                    Float tanThetaH2 = local_coord::tanTheta2(wh);
                    if (std::isinf(tanThetaH2)) {
                        return 0.;
                    }

                    Float cosThetaH4 = cosThetaH * cosThetaH * cosThetaH * cosThetaH;
                    Float item = _alpha_g * _alpha_g + tanThetaH2;
                    return (_alpha_g * _alpha_g) / (math::PI * cosThetaH4 * item * item);
                }

                Float GGXDistribution::G(const Vector3F &wo, const Normal3F &wh) const {
                    if (DOT(wo, wh) * DOT(wo, Vector3F(0, 1, 0)) < 0.) {
                        return 0.;
                    }

                    Float tanThetaO2 = local_coord::tanTheta2(wo);
                    if (std::isinf(tanThetaO2)) {
                        return 0.;
                    }

                    if (tanThetaO2 == 0.0f) {
                        return 1.0f;
                    }

                    Float alpha2 = _alpha_g * _alpha_g;
                    return 2.0 / (1 + std::sqrt(1 + alpha2 * tanThetaO2));
                }

                Float GGXDistribution::G(const Vector3F &wo, const Vector3F &wi, const Normal3F &wh) const {
                    return G(wo, wh) * G(wi, wh);
                }

                Vector3F GGXDistribution::sampleWh(const Vector3F &wo, Sampler *sampler) const {
                    Float u = sampler->sample1D();
                    Float v = sampler->sample1D();

                    Float phi = 2 * math::PI * u;
                    Float theta = std::atan((_alpha_g * sqrt(v)) / (std::sqrt(1 - v)));

                    Float cosTheta = std::cos(theta);
                    Float sinTheta = std::sin(theta);
                    Float cosPhi = std::cos(phi);
                    Float sinPhi = std::sin(phi);
                    Vector3F wh = Vector3F(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi);
                    if (wo.y * wh.y < 0) {
                        wh.y *= -1;
                    }
                    return NORMALIZE(wh);
                }

                Float GGXDistribution::lambda(const Vector3F &wo, const Normal3F &wh) const {
                    if (DOT(wo, wh) * DOT(wo, Vector3F(0, 1, 0)) < 0.) {
                        return 0.;
                    }

                    Float tanThetaO2 = local_coord::tanTheta2(wo);
                    if (std::isinf(tanThetaO2)) {
                        return 0.;
                    }

                    Float alpha2 = _alpha_g * _alpha_g;
                    return (std::sqrt(1 + alpha2 * tanThetaO2) - 1) / 2;
                }
            }
        }
    }
}