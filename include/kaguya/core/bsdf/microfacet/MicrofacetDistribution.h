//
// Created by Storm Phoenix on 2021/5/7.
//

#ifndef KAGUYA_MICROFACETDISTRIBUTION_H
#define KAGUYA_MICROFACETDISTRIBUTION_H

#include <kaguya/Common.h>
#include <kaguya/core/Core.h>
#include <kaguya/sampler/Sampler.h>

namespace kaguya {
    namespace core {
        namespace microfacet {
            using sampler::Sampler;

            class MicrofacetDistribution {
            public:
                // Normal distribution function D(wh)
                virtual Float D(const Normal3F &wh) const = 0;

                // Mask and shadowing function G(wi, wh)
                // G(wi, wh) is independent of wh, so rewrite G(wi, wh) to G(wi)
                virtual Float G(const Vector3F &wo) const;

                // Mask and shadowing function G(wo, wi)
                // Which gives the fraction of microfacets in a differential area that are visible from
                //  both directions w_o and w_i
                virtual Float G(const Vector3F &wo, const Vector3F &wi) const;

                // Sample wh
                virtual Vector3F sampleWh(const Vector3F &wo, Sampler *sampler) const = 0;

                // Sample wh pdf
                virtual Float samplePdf(const Vector3F &wo, const Vector3F &wh) const;

            protected:
                // Auxiliary function Lambda(wi) for G(wi)
                virtual Float lambda(const Vector3F &wo) const = 0;
            };

        }
    }
}

#endif //KAGUYA_MICROFACETDISTRIBUTION_H
