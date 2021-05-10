//
// Created by Storm Phoenix on 2021/5/7.
//

#ifndef KAGUYA_BXDFMICROFACETREFLECTION_H
#define KAGUYA_BXDFMICROFACETREFLECTION_H

#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/core/bsdf/microfacet/MicrofacetDistribution.h>
#include <kaguya/core/bsdf/fresnel/Fresnel.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            using microfacet::MicrofacetDistribution;
            using fresnel::Fresnel;

            class BXDFMicrofacetReflection : public BXDF {
            public:
                BXDFMicrofacetReflection(const Spectrum &specularReflectance,
                                         const MicrofacetDistribution *microfacetDistribution,
                                         const Fresnel *fresnel);

                virtual Spectrum f(const Vector3F &wo, const Vector3F &wi) const override;

                virtual Spectrum sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                         Sampler *const sampler, BXDFType *sampleType) override;

                virtual Float samplePdf(const Vector3F &wo, const Vector3F &wi) const override;

            private:
                const Spectrum _specularReflectance;
                const MicrofacetDistribution *_microfacetDistribution;
                const Fresnel *_fresnel;
            };
        }
    }
}

#endif //KAGUYA_BXDFMICROFACETREFLECTION_H
