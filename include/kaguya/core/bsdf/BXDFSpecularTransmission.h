//
// Created by Storm Phoenix on 2020/10/15.
//

#ifndef KAGUYA_BXDFSPECULARTRANSMISSION_H
#define KAGUYA_BXDFSPECULARTRANSMISSION_H

#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/core/bsdf/FresnelDielectric.h>

namespace kaguya {
    namespace core {

        /**
         * 高光类型，用于折射
         */
        class BXDFSpecularTransmission : public BXDF {
        public:
            /**
             * TODO 说明 RADIANCE 的作用
             * @param albedo
             * @param thetaI
             * @param thetaT
             * @param fresnel
             * @param mode
             */
            BXDFSpecularTransmission(const Spectrum &albedo, double thetaI, double thetaT,
                                     FresnelDielectric *fresnel, TransportMode mode = TransportMode::RADIANCE);

            virtual Spectrum f(const Vector3 &wo, const Vector3 &wi) const override;

            virtual Spectrum sampleF(const Vector3 &wo, Vector3 *wi, double *pdf,
                                     const Sampler *const sampler1D) override;

            virtual double samplePdf(const Vector3 &wo, const Vector3 &wi) const override;

        private:
            const Spectrum _albedo;
            const double _thetaI;
            const double _thetaT;
            // 光线传输模式
            const TransportMode _mode;
            FresnelDielectric *_fresnel = nullptr;
        };

    }
}

#endif //KAGUYA_BXDFSPECULARTRANSMISSION_H
