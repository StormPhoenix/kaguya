//
// Created by Storm Phoenix on 2021/5/7.
//

#include <kaguya/math/Math.h>
#include <kaguya/core/bsdf/microfacet/BeckmannDistribution.h>

namespace kaguya {
    namespace core {
        namespace microfacet {
            using namespace math;

            /*
             * Reference from pbrt
             * Further reading: https://www.zhihu.com/question/395625914
             */
            inline Float Erf(Float x) {
                // constants
                Float a1 = 0.254829592f;
                Float a2 = -0.284496736f;
                Float a3 = 1.421413741f;
                Float a4 = -1.453152027f;
                Float a5 = 1.061405429f;
                Float p = 0.3275911f;

                // Save the sign of x
                int sign = 1;
                if (x < 0) sign = -1;
                x = std::abs(x);

                // A&S formula 7.1.26
                Float t = 1 / (1 + p * x);
                Float y =
                        1 -
                        (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * std::exp(-x * x);

                return sign * y;
            }

            inline Float ErfInv(Float x) {
                Float w, p;
                x = clamp(x, -.99999f, .99999f);
                w = -std::log((1 - x) * (1 + x));
                if (w < 5) {
                    w = w - 2.5f;
                    p = 2.81022636e-08f;
                    p = 3.43273939e-07f + p * w;
                    p = -3.5233877e-06f + p * w;
                    p = -4.39150654e-06f + p * w;
                    p = 0.00021858087f + p * w;
                    p = -0.00125372503f + p * w;
                    p = -0.00417768164f + p * w;
                    p = 0.246640727f + p * w;
                    p = 1.50140941f + p * w;
                } else {
                    w = std::sqrt(w) - 3;
                    p = -0.000200214257f;
                    p = 0.000100950558f + p * w;
                    p = 0.00134934322f + p * w;
                    p = -0.00367342844f + p * w;
                    p = 0.00573950773f + p * w;
                    p = -0.0076224613f + p * w;
                    p = 0.00943887047f + p * w;
                    p = 1.00167406f + p * w;
                    p = 2.83297682f + p * w;
                }
                return p * x;
            }

            static void BeckmannSample11(Float cosThetaI, Float U1, Float U2,
                                         Float *slope_x, Float *slope_y) {
                /* Special case (normal incidence) */
                if (cosThetaI > .9999) {
                    Float r = std::sqrt(-std::log(1.0f - U1));
                    Float sinPhi = std::sin(2 * PI * U2);
                    Float cosPhi = std::cos(2 * PI * U2);
                    *slope_x = r * cosPhi;
                    *slope_y = r * sinPhi;
                    return;
                }

                /* The original inversion routine from the paper contained
                   discontinuities, which causes issues for QMC integration
                   and techniques like Kelemen-style MLT. The following code
                   performs a numerical inversion with better behavior */
                Float sinThetaI =
                        std::sqrt(std::max((Float) 0, (Float) 1 - cosThetaI * cosThetaI));
                Float tanThetaI = sinThetaI / cosThetaI;
                Float cotThetaI = 1 / tanThetaI;

                /* Search interval -- everything is parameterized
                   in the Erf() domain */
                Float a = -1, c = Erf(cotThetaI);
                Float sample_x = std::max(U1, (Float) 1e-6f);

                /* Start with a good initial guess */
                // Float b = (1-sample_x) * a + sample_x * c;

                /* We can do better (inverse of an approximation computed in
                 * Mathematica) */
                Float thetaI = std::acos(cosThetaI);
                Float fit = 1 + thetaI * (-0.876f + thetaI * (0.4265f - 0.0594f * thetaI));
                Float b = c - (1 + c) * std::pow(1 - sample_x, fit);

                /* Normalization factor for the CDF */
                static const Float SQRT_PI_INV = 1.f / std::sqrt(PI);
                Float normalization =
                        1 /
                        (1 + c + SQRT_PI_INV * tanThetaI * std::exp(-cotThetaI * cotThetaI));

                int it = 0;
                while (++it < 10) {
                    /* Bisection criterion -- the oddly-looking
                       Boolean expression are intentional to check
                       for NaNs at little additional cost */
                    if (!(b >= a && b <= c)) b = 0.5f * (a + c);

                    /* Evaluate the CDF and its derivative
                       (i.e. the density function) */
                    Float invErf = ErfInv(b);
                    Float value =
                            normalization *
                            (1 + b + SQRT_PI_INV * tanThetaI * std::exp(-invErf * invErf)) -
                            sample_x;
                    Float derivative = normalization * (1 - invErf * tanThetaI);

                    if (std::abs(value) < 1e-5f) break;

                    /* Update bisection intervals */
                    if (value > 0)
                        c = b;
                    else
                        a = b;

                    b -= value / derivative;
                }

                /* Now convert back into a slope value */
                *slope_x = ErfInv(b);

                /* Simulate Y component */
                *slope_y = ErfInv(2.0f * std::max(U2, (Float) 1e-6f) - 1.0f);

                ASSERT(!std::isinf(*slope_x), "slope_x is Inf. ");
                ASSERT(!std::isnan(*slope_x), "slope_x is NaN. ");
                ASSERT(!std::isinf(*slope_y), "slope_y is Inf. ");
                ASSERT(!std::isnan(*slope_y), "slope_y is NaN. ");
            }

            static Vector3F BeckmannSample(const Vector3F &wi, Float alpha_x, Float alpha_y,
                                           Float U1, Float U2) {
                // 1. stretch wi
                Vector3F wiStretched =
                        NORMALIZE(Vector3F(alpha_x * wi.x, alpha_y * wi.y, wi.z));

                // 2. simulate P22_{wi}(x_slope, y_slope, 1, 1)
                Float slope_x, slope_y;
                BeckmannSample11(local_coord::cosTheta(wiStretched), U1, U2, &slope_x, &slope_y);

                // 3. rotate
                Float tmp = local_coord::cosPhi(wiStretched) * slope_x - local_coord::sinPhi(wiStretched) * slope_y;
                slope_y = local_coord::sinPhi(wiStretched) * slope_x + local_coord::cosPhi(wiStretched) * slope_y;
                slope_x = tmp;

                // 4. unstretch
                slope_x = alpha_x * slope_x;
                slope_y = alpha_y * slope_y;

                // 5. compute normal
                return NORMALIZE(Vector3F(-slope_x, -slope_y, 1.f));
            }

            BeckmannDistribution::BeckmannDistribution(Float alpha) :
                    _alphaX(alpha), _alphaZ(alpha) {}

            BeckmannDistribution::BeckmannDistribution(Float alphaX, Float alphaZ) :
                    _alphaX(_alphaX), _alphaZ(alphaZ) {}

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
                Float tanThetaWo = local_coord::tanTheta(wo);
                if (tanThetaWo == 0. || std::isinf(tanThetaWo)) {
                    return 0.;
                }
                tanThetaWo = std::abs(tanThetaWo);

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
                Vector3F wh;
                bool flip = wo.z < 0;
                wh = BeckmannSample(flip ? -wo : wo, _alphaX, _alphaZ, sampler->sample1D(), sampler->sample1D());
                if (flip) wh = -wh;
                return wh;
            }
        }
    }
}