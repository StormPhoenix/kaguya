//
// Created by Storm Phoenix on 2020/10/15.
//

#ifndef KAGUYA_BXDFSPECULARTRANSMISSION_H
#define KAGUYA_BXDFSPECULARTRANSMISSION_H

#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/core/bsdf/Fresnel.h>

namespace kaguya {
    namespace core {

        /**
         * 高光类型，用于折射
         */
        class BXDFSpecularTransmission : public BXDF {
        public:
            BXDFSpecularTransmission(const Spectrum &albedo, double thetaI, double thetaT,
                                     std::shared_ptr<Fresnel> fresnel);

            virtual Spectrum f(const Vector3 &wo, const Vector3 &wi) const override;

            virtual Spectrum sampleF(const Vector3 &wo, Vector3 *wi, double *pdf) override;

            virtual double samplePdf(const Vector3 &wo, const Vector3 &wi) const override;

        private:
            const Spectrum _albedo;
            const double _thetaI;
            const double _thetaT;
            std::shared_ptr<Fresnel> _fresnel = nullptr;
        };

    }
}

#endif //KAGUYA_BXDFSPECULARTRANSMISSION_H
