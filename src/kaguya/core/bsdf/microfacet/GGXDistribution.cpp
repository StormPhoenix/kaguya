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
                    // Ignore wh direction eg. if (cosThetaH < 0) -> return zero.
                    if (cosThetaH == 0) {
                        return 0;
                    }

                    Float tanTheta2Wh = local_coord::tanTheta2(wh);
                    if (std::isinf(tanTheta2Wh)) {
                        return 0.;
                    }

                    Float cosThetaH4 = cosThetaH * cosThetaH * cosThetaH * cosThetaH;
                    Float item = _alpha_g * _alpha_g + tanTheta2Wh;
                    return (_alpha_g * _alpha_g) / (math::PI * cosThetaH4 * item * item);
                }

                Float GGXDistribution::G(const Vector3F &v, const Normal3F &wh) const {
                    if (DOT(v, wh) * DOT(v, Vector3F(0, 1, 0)) < 0.) {
                        return 0.;
                    }

                    Float tanTheta2V = local_coord::tanTheta2(v);
                    if (std::isinf(tanTheta2V)) {
                        return 0.;
                    }

                    if (tanTheta2V == 0.0f) {
                        return 1.0f;
                    }

                    Float alpha2 = _alpha_g * _alpha_g;
                    return 2.0 / (1 + std::sqrt(1 + alpha2 * tanTheta2V));
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
                    // Correct wh to the same hemi-sphere
                    if (wo.y * wh.y < 0) {
                        wh *= -1;
                    }
                    return NORMALIZE(wh);
                }

                Float GGXDistribution::lambda(const Vector3F &v, const Normal3F &wh) const {
                    if (DOT(v, wh) * DOT(v, Vector3F(0, 1, 0)) < 0.) {
                        return 0.;
                    }

                    Float tanTheta2V = local_coord::tanTheta2(v);
                    if (std::isinf(tanTheta2V)) {
                        return 0.;
                    }

                    Float alpha2 = _alpha_g * _alpha_g;
                    return (std::sqrt(1 + alpha2 * tanTheta2V) - 1) / 2;
                }
            }
        }
    }
}