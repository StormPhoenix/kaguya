//
// Created by Storm Phoenix on 2021/5/7.
//

#include <kaguya/math/Math.h>
#include <kaguya/core/bsdf/microfacet/BeckmannDistribution.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            namespace microfacet {
                using namespace math;

                // Reference PBRT

                BeckmannDistribution::BeckmannDistribution(Float alpha) :
                        _alphaX(alpha), _alphaZ(alpha) {}

                BeckmannDistribution::BeckmannDistribution(Float alphaX, Float alphaZ) :
                        _alphaX(alphaX), _alphaZ(alphaZ) {}

                Float BeckmannDistribution::D(const Normal3F &wh) const {
                    Float cosThetaWh2 = local_coord::cosTheta2(wh);
                    if (cosThetaWh2 == 0.) {
                        return 0.;
                    }

                    Float tanThetaWh2 = local_coord::tanTheta2(wh);
                    if (std::isinf(tanThetaWh2)) {
                        return 0.;
                    }

                    Float cosPhiWh2 = local_coord::cosPhi2(wh);
                    Float sinPhiWh2 = local_coord::sinPhi2(wh);

                    return std::exp(-tanThetaWh2 * (cosPhiWh2 / (_alphaX * _alphaX) + sinPhiWh2 / (_alphaZ * _alphaZ)))
                           / (PI * _alphaX * _alphaZ * cosThetaWh2 * cosThetaWh2);
                }

                Float BeckmannDistribution::lambda(const Vector3F &wo) const {
                    Float tanThetaWo = std::abs(local_coord::tanTheta(wo));
                    if (std::isinf(tanThetaWo)) {
                        return 0.;
                    }

                    Float cosPhiWo2 = local_coord::cosPhi2(wo);
                    Float sinPhiWo2 = local_coord::sinPhi2(wo);
                    Float alpha = std::sqrt(cosPhiWo2 / (_alphaX * _alphaX) + sinPhiWo2 / (_alphaZ * _alphaZ));
                    Float a = 1. / (alpha * tanThetaWo);
                    // TODO ?
                    if (a >= 1.6f) {
                        return 0;
                    }

                    // Rational polynomial approximation
                    return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
                }

                Vector3F BeckmannDistribution::sampleWh(const Vector3F &wo, Sampler *sampler) const {
                    // Reference from PBRT
                    Float u = sampler->sample1D();
                    Float v = sampler->sample1D();
                    Float tanTheta2, phi;
                    if (_alphaX == _alphaZ) {
                        Float logSample = std::log(1 - (u == 1. ? (u - epsilon) : u));
                        ASSERT(!std::isinf(logSample), "BeckmannDistribution::sampleWh() Inf error. ");
                        tanTheta2 = -_alphaX * _alphaX * logSample;
                        phi = v * 2 * PI;
                    } else {
                        Float logSample = std::log(1 - (u == 1. ? (u - epsilon) : u));
                        ASSERT(!std::isinf(logSample), "BeckmannDistribution::sampleWh() Inf error. ");
                        phi = std::atan(_alphaZ / _alphaX *
                                        std::tan(2 * PI * v + 0.5f * PI));
                        if (v > 0.5f) phi += PI;
                        Float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
                        Float alphax2 = _alphaX * _alphaX, alphaz2 = _alphaZ * _alphaZ;
                        tanTheta2 = -logSample /
                                    (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphaz2);
                    }

                    // Map sampled Beckmann angles to normal direction _wh_
                    Float cosTheta = 1 / std::sqrt(1 + tanTheta2);
                    Float sinTheta = std::sqrt(std::max((Float) 0, 1 - cosTheta * cosTheta));
                    Vector3F wh = Vector3F(sinTheta * std::cos(phi), cosTheta, sinTheta * std::sin(phi));
                    if (wo.y * wh.y < 0) {
                        wh = -wh;
                    };
                    return wh;
                }
            }
        }
    }
}