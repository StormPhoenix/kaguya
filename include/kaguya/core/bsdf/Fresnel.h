//
// Created by Storm Phoenix on 2020/10/15.
//

#ifndef KAGUYA_FRESNEL_H
#define KAGUYA_FRESNEL_H

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace kaguya {
    namespace core {

        class Fresnel {
        public:
            /**
             * 计算辐射反射量
             * @param cosineTheta 入射角余弦值
             * @return
             */
            virtual Spectrum fresnel(double cosineTheta) = 0;

            virtual ~Fresnel() {};
        };

        /**
         * 计算非导体（玻璃、水晶等）的反射概率
         * @param cosineI 值的正负用于表示内部、外部射入
         * @param thetaI
         * @param thetaT
         * @return
         */
        inline double fresnelDielectric(double cosineI, double thetaI, double thetaT) {
            cosineI = clamp(cosineI, -1, 1);
            if (cosineI < 0) {
                // 内部射入
                cosineI = std::abs(cosineI);
                std::swap(thetaI, thetaT);
            }

            double sineI = std::sqrt(std::max(0.0, 1 - std::pow(cosineI, 2)));
            double sineT = sineI * (thetaI / thetaT);

            if (sineT >= 1) {
                // 全反射
                return 1.0f;
            }

            double cosineT = std::sqrt(std::max(0.0, (1 - std::pow(sineT, 2))));
            // 计算 R_parallel
            double parallelR = ((thetaT * cosineI) - (thetaI * cosineT)) /
                               ((thetaT * cosineI) + (thetaI * cosineT));
            double perpendicularR = ((thetaI * cosineI) - (thetaT * cosineT)) /
                                    ((thetaI * cosineI) + (thetaT * cosineT));
            return 0.5 * (parallelR * parallelR + perpendicularR * perpendicularR);
        }

        inline Spectrum fresnelConductor(double cosineI, const Spectrum &thetaI,
                                         const Spectrum &thetaT, const Spectrum &k) {
            cosineI = clamp(cosineI, -1, 1);
            Spectrum theta = thetaT / thetaI;
            Spectrum thetaK = k / thetaI;

            double cosine2 = cosineI * cosineI;
            double sine2 = 1 - sine2;

            Spectrum theta2 = theta * theta;
            Spectrum thetaK2 = thetaK * thetaK;

            Spectrum item0 = theta2 - thetaK2 - sine2;
            Spectrum a2Andb2 = sqrt(item0 * item0 + 4 * theta2 * thetaK2);
            // TODO 不懂此处的 a 是如何计算得到的
            Spectrum a = sqrt(0.5 * (a2Andb2 + item0));
            Spectrum item1 = a2Andb2 + cosine2;
            Spectrum item2 = 2 * a * cosineI;

            Spectrum perpendicularR = (item1 - item2) /
                                      (item1 + item2);

            Spectrum item3 = cosine2 * a2Andb2 + sine2 * sine2;
            Spectrum item4 = 2 * a * cosineI * sine2;

            Spectrum parallelR = perpendicularR * (item3 - item4) /
                                 (item3 + item4);
            return 0.5 * (perpendicularR + parallelR);
        }
    }
}

#endif //KAGUYA_FRESNEL_H
