//
// Created by Storm Phoenix on 2021/5/7.
//

#ifndef KAGUYA_BXDFMICROFACETTRANSMISSION_H
#define KAGUYA_BXDFMICROFACETTRANSMISSION_H

#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/core/bsdf/fresnel/FresnelDielectric.h>
#include <kaguya/core/bsdf/microfacet/MicrofacetDistribution.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            using namespace fresnel;
            using namespace microfacet;

            class BXDFMicrofacetTransmission : public BXDF {
            public:
                BXDFMicrofacetTransmission(Float etaI, Float etaT, const Spectrum &Kt,
                                           const MicrofacetDistribution *microfacetDistribution,
                                           TransportMode mode = TransportMode::RADIANCE);

                virtual Spectrum f(const Vector3F &wo, const Vector3F &wi) const override;

                virtual Spectrum sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                         Sampler *const sampler, BXDFType *sampleType) override;

                virtual Float samplePdf(const Vector3F &wo, const Vector3F &wi) const override;

            private:
                Float _etaI, _etaT;
                const Spectrum _Kt;
                const TransportMode _mode;
                const MicrofacetDistribution *_microfacetDistribution;
                const FresnelDielectric _fresnel;
            };
        }
    }
}

#endif //KAGUYA_BXDFMICROFACETTRANSMISSION_H
