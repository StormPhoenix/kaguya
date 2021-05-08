//
// Created by Storm Phoenix on 2020/10/16.
//

#ifndef KAGUYA_BXDFSPECULAR_H
#define KAGUYA_BXDFSPECULAR_H

#include <kaguya/core/bsdf/BXDF.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            /**
             * 高光类型，包含反射、折射
             */
            class BXDFSpecular : public BXDF {
            public:
                BXDFSpecular(const Spectrum &albedo, Float thetaI, Float thetaT,
                             TransportMode mode = TransportMode::RADIANCE);

                // TODO add constructor method for Fresnel *

                virtual Spectrum f(const Vector3F &wo, const Vector3F &wi) const override;

                virtual Spectrum sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                         Sampler *const sampler, BXDFType *sampleType) override;

                virtual Float samplePdf(const Vector3F &wo, const Vector3F &wi) const override;

            private:
                Float _thetaI;
                Float _thetaT;
                Spectrum _albedo;
                const TransportMode _mode;
            };
        }
    }
}

#endif //KAGUYA_BXDFSPECULAR_H
