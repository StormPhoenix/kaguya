//
// Created by Storm Phoenix on 2020/10/15.
//

#ifndef KAGUYA_FRESNEL_H
#define KAGUYA_FRESNEL_H

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace kaguya {
    namespace core {
        namespace bsdf {
            namespace fresnel {
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

                inline Spectrum fresnelConductor(Float cosineI, const Spectrum &thetaI,
                                                 const Spectrum &thetaT, const Spectrum &k) {
                    cosineI = math::clamp(cosineI, -1, 1);
                    Spectrum theta = thetaT / thetaI;
                    Spectrum thetaK = k / thetaI;

                    Float cosine2 = cosineI * cosineI;
                    Float sine2 = 1 - cosine2;

                    Spectrum theta2 = theta * theta;
                    Spectrum thetaK2 = thetaK * thetaK;

                    Spectrum item0 = theta2 - thetaK2 - sine2;
                    Spectrum a2Andb2 = sqrt(item0 * item0 + 4 * theta2 * thetaK2);
                    // TODO 不懂此处的 a 是如何计算得到的
                    Spectrum a = sqrt(0.5 * (a2Andb2 + item0));
                    Spectrum item1 = a2Andb2 + cosine2;
                    Spectrum item2 = 2. * a * cosineI;

                    Spectrum perpendicularR = (item1 - item2) /
                                              (item1 + item2);

                    Spectrum item3 = cosine2 * a2Andb2 + sine2 * sine2;
                    Spectrum item4 = item2 * sine2;

                    Spectrum parallelR = perpendicularR * (item3 - item4) /
                                         (item3 + item4);
                    return 0.5 * (perpendicularR + parallelR);
                }
            }
        }
    }
}

#endif //KAGUYA_FRESNEL_H
