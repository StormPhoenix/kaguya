//
// Created by Storm Phoenix on 2021/5/14.
//

#ifndef KAGUYA_BXDFMICROFACET_H
#define KAGUYA_BXDFMICROFACET_H

#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/core/bsdf/microfacet/MicrofacetDistribution.h>

namespace kaguya {
    namespace core {
        namespace bsdf {

            using namespace microfacet;

            class BXDFMicrofacet : public BXDF {
            public:
                BXDFMicrofacet(const Spectrum &Ks, const Spectrum &Kt,
                               Float etaI, Float etaT, const MicrofacetDistribution *distribution,
                               const TransportMode mode);

                virtual Spectrum sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                         Sampler *const sampler, BXDFType *sampleType) override;

                virtual Spectrum f(const Vector3F &wo, const Vector3F &wi) const override;

                virtual Float samplePdf(const Vector3F &wo, const Vector3F &wi) const override;

            private:
                Float _etaT, _etaI;
                const Spectrum _Ks, _Kt;
                const TransportMode _mode;
                const MicrofacetDistribution *_microfacetDistribution;
            };
        }
    }
}

#endif //KAGUYA_BXDFMICROFACET_H
