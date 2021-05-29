//
// Created by Storm Phoenix on 2020/10/15.
//

#ifndef KAGUYA_FRESNEL_H
#define KAGUYA_FRESNEL_H

#include <kaguya/core/core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace RENDER_NAMESPACE {
    namespace core {
        namespace bsdf {
            namespace fresnel {

                // References https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
                inline Spectrum fresnelConductor(Float cosThetaI, const Spectrum &etaI,
                                                 const Spectrum &etaT, const Spectrum &k) {
                    cosThetaI = math::clamp(cosThetaI, -1, 1);
                    Spectrum eta = etaT / etaI;
                    Spectrum K = k / etaI;

                    Float cosTheta2 = cosThetaI * cosThetaI;
                    Float sinTheta2 = 1 - cosTheta2;

                    Spectrum eta2 = eta * eta;
                    Spectrum K2 = K * K;

                    Spectrum item0 = eta2 - K2 - sinTheta2;
                    Spectrum a2Andb2 = sqrt(item0 * item0 + 4 * eta2 * K2);
                    Spectrum a = sqrt(0.5 * (a2Andb2 + item0));
                    Spectrum item1 = a2Andb2 + cosTheta2;
                    Spectrum item2 = 2. * a * cosThetaI;

                    Spectrum perpendicularR = (item1 - item2) /
                                              (item1 + item2);

                    Spectrum item3 = cosTheta2 * a2Andb2 + sinTheta2 * sinTheta2;
                    Spectrum item4 = item2 * sinTheta2;

                    Spectrum parallelR = perpendicularR * (item3 - item4) /
                                         (item3 + item4);
                    return 0.5 * (perpendicularR + parallelR);
                }

                // TODO Add spectrum mode
                inline Float fresnelDielectric(Float cosThetaI, Float etaI, Float etaT) {
                    cosThetaI = math::clamp(cosThetaI, -1, 1);
                    if (cosThetaI < 0) {
                        // Traver form inner
                        cosThetaI = std::abs(cosThetaI);
                        std::swap(etaI, etaT);
                    }

                    Float sinThetaI = std::sqrt((std::max)(Float(0.), Float(1 - std::pow(cosThetaI, 2))));
                    Float sinThetaT = sinThetaI * (etaI / etaT);

                    if (sinThetaT >= 1) {
                        // Totally reflection
                        return 1.0f;
                    }

                    Float cosThetaT = std::sqrt((std::max)(Float(0.), Float(1 - std::pow(sinThetaT, 2))));
                    // 计算 R_parallel
                    Float parallelR = ((etaT * cosThetaI) - (etaI * cosThetaT)) /
                                      ((etaT * cosThetaI) + (etaI * cosThetaT));
                    Float perpendicularR = ((etaI * cosThetaI) - (etaT * cosThetaT)) /
                                           ((etaI * cosThetaI) + (etaT * cosThetaT));
                    return 0.5 * (parallelR * parallelR + perpendicularR * perpendicularR);
                }

                // Fresnel schlick approximation
                inline Float fresnelSchlick(Float cosTheta, Float eta) {
                    if (cosTheta < 0) {
                        eta = 1 / eta;
                        cosTheta = -cosTheta;
                    }

                    Float sine = std::sqrt((std::max)(Float(0.), Float(1 - std::pow(cosTheta, 2))));
                    if (sine * eta >= 1) {
                        return 1.0;
                    }

                    auto r0 = (1 - eta) / (1 + eta);
                    r0 = r0 * r0;
                    return r0 + (1 - r0) * pow((1 - cosTheta), 5);
                }

                class Fresnel {
                public:
                    /**
                     * 计算辐射反射量
                     * @param cosineTheta 入射角余弦值
                     * @return
                     */
                    virtual Spectrum fresnel(Float cosineTheta) const = 0;

                    virtual ~Fresnel() {};
                };


            }
        }
    }
}

#endif //KAGUYA_FRESNEL_H
