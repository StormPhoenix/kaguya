//
// Created by Storm Phoenix on 2021/5/10.
//

#ifndef KAGUYA_BXDFGLOSSYDIFFUSEREFLECTION_H
#define KAGUYA_BXDFGLOSSYDIFFUSEREFLECTION_H

#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/core/bsdf/microfacet/MicrofacetDistribution.h>

namespace kaguya {
    namespace core {
        namespace bsdf {

            using microfacet::MicrofacetDistribution;

            class BXDFGlossyDiffuseReflection : public BXDF {
            public:
                BXDFGlossyDiffuseReflection(const Spectrum &Rd, const Spectrum &Rs,
                                            const MicrofacetDistribution *dist);

                virtual Spectrum f(const Vector3F &wo, const Vector3F &wi) const override;

                virtual Spectrum sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                         Sampler *const sampler, BXDFType *sampleType) override;

                virtual Float samplePdf(const Vector3F &wo, const Vector3F &wi) const override;

            private:
                const Spectrum _Rd;
                const Spectrum _Rs;
                const MicrofacetDistribution *_distribution;
            };
        }
    }
}
#endif //KAGUYA_BXDFGLOSSYDIFFUSEREFLECTION_H
