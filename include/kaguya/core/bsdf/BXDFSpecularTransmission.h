//
// Created by Storm Phoenix on 2020/10/15.
//

#ifndef KAGUYA_BXDFSPECULARTRANSMISSION_H
#define KAGUYA_BXDFSPECULARTRANSMISSION_H

#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/core/bsdf/fresnel/FresnelDielectric.h>

namespace RENDER_NAMESPACE {
    namespace core {
        namespace bsdf {
            using fresnel::FresnelDielectric;

            class BXDFSpecularTransmission : public BXDF {
            public:
                BXDFSpecularTransmission(const Spectrum &albedo, Float thetaI, Float thetaT,
                                         FresnelDielectric *fresnel, TransportMode mode = TransportMode::RADIANCE);

                virtual Spectrum f(const Vector3F &wo, const Vector3F &wi) const override;

                virtual Spectrum sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                         Sampler *const sampler, BXDFType *sampleType) override;

                virtual Float samplePdf(const Vector3F &wo, const Vector3F &wi) const override;

            private:
                const Spectrum _albedo;
                const Float _thetaI;
                const Float _thetaT;
                // 光线传输模式
                const TransportMode _mode;
                FresnelDielectric *_fresnel = nullptr;
            };
        }
    }
}

#endif //KAGUYA_BXDFSPECULARTRANSMISSION_H
