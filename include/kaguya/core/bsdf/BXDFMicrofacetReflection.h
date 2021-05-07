//
// Created by Storm Phoenix on 2021/5/7.
//

#ifndef KAGUYA_BXDFMICROFACETREFLECTION_H
#define KAGUYA_BXDFMICROFACETREFLECTION_H

#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/core/bsdf/microfacet/MicrofacetDistribution.h>
#include <kaguya/core/bsdf/Fresnel.h>

namespace kaguya {
    namespace core {
        using microfacet::MicrofacetDistribution;

        class BXDFMicrofacetReflection : public BXDF {
        public:
            BXDFMicrofacetReflection(const Spectrum &reflectance,
                                     const MicrofacetDistribution *microfacetDistribution,
                                     const Fresnel *fresnel);

            virtual Spectrum f(const Vector3F &wo, const Vector3F &wi) const override;

            virtual Spectrum sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                     Sampler *const sampler, BXDFType *sampleType) override;

            virtual Float samplePdf(const Vector3F &wo, const Vector3F &wi) const override;

        private:
            const Spectrum _reflectance;
            const MicrofacetDistribution *_microfacetDistribution;
            const Fresnel *_fresnel;
        };
    }
}

#endif //KAGUYA_BXDFMICROFACETREFLECTION_H
