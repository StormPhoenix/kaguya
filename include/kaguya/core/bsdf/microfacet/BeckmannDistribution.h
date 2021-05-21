//
// Created by Storm Phoenix on 2021/5/7.
//

#ifndef KAGUYA_BECKMANNDISTRIBUTION_H
#define KAGUYA_BECKMANNDISTRIBUTION_H

#include <kaguya/core/bsdf/microfacet/MicrofacetDistribution.h>

namespace RENDER_NAMESPACE {
    namespace core {
        namespace bsdf {
            namespace microfacet {

                class BeckmannDistribution : public MicrofacetDistribution {
                public:
                    BeckmannDistribution(Float alpha);

                    BeckmannDistribution(Float alphaX, Float alphaZ);

                    virtual Float D(const Normal3F &wh) const override;

                    virtual Vector3F sampleWh(const Vector3F &wo, Sampler *sampler) const override;

                protected:
                    virtual Float lambda(const Vector3F &wo, const Normal3F &wh) const override;

                protected:
                    Float _alphaX, _alphaZ;
                };

            }
        }
    }
}

#endif //KAGUYA_BECKMANNDISTRIBUTION_H
