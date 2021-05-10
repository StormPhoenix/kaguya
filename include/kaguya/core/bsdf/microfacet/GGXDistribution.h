//
// Created by Storm Phoenix on 2021/5/10.
//

#ifndef KAGUYA_GGXDISTRIBUTION_H
#define KAGUYA_GGXDISTRIBUTION_H

#include <kaguya/core/bsdf/microfacet/MicrofacetDistribution.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            namespace microfacet {
                class GGXDistribution : public MicrofacetDistribution {
                public:
                    // TODO
                    virtual Float D(const Normal3F &wh) const override;
                };
            }
        }
    }
}
#endif //KAGUYA_GGXDISTRIBUTION_H
