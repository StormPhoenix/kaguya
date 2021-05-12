//
// Created by Storm Phoenix on 2021/5/10.
//
// Reference from: Microfacet Model for Refraction through Rough Surfaces

#ifndef KAGUYA_GGXDISTRIBUTION_H
#define KAGUYA_GGXDISTRIBUTION_H

#include <kaguya/core/bsdf/microfacet/MicrofacetDistribution.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            namespace microfacet {
                class GGXDistribution : public MicrofacetDistribution {
                public:
                    GGXDistribution(Float alpha_g);

                    virtual Float D(const Normal3F &wh) const override;

                    virtual Float G(const Vector3F &wo, const Normal3F &wh) const override;

                    virtual Float G(const Vector3F &wo, const Vector3F &wi, const Normal3F &wh) const override;

                    virtual Vector3F sampleWh(const Vector3F &wo, Sampler *sampler) const override;

                protected:
                    virtual Float lambda(const Vector3F &wo, const Normal3F &wh) const override;

                private:
                    // Width parameter
                    Float _alpha_g;
                };
            }
        }
    }
}
#endif //KAGUYA_GGXDISTRIBUTION_H
