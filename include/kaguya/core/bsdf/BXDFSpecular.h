//
// Created by Storm Phoenix on 2020/10/16.
//

#ifndef KAGUYA_BXDFSPECULAR_H
#define KAGUYA_BXDFSPECULAR_H

#include <kaguya/core/bsdf/BXDF.h>

namespace kaguya {
    namespace core {

        /**
         * 高光类型，包含反射、折射
         */
        class BXDFSpecular : public BXDF {
        public:
            BXDFSpecular(const Spectrum &albedo, double thetaI, double thetaT,
                         TransportMode mode = TransportMode::RADIANCE);

            virtual Spectrum f(const Vector3 &wo, const Vector3 &wi) const override;

            virtual Spectrum sampleF(const Vector3 &wo, Vector3 *wi, double *pdf,
                                     const Sampler *const sampler1D) override;

            virtual double samplePdf(const Vector3 &wo, const Vector3 &wi) const override;

        private:
            double _thetaI;
            double _thetaT;
            Spectrum _albedo;
            const TransportMode _mode;
        };

    }
}

#endif //KAGUYA_BXDFSPECULAR_H
