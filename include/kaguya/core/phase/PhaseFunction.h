//
// Created by Storm Phoenix on 2020/11/20.
//

#ifndef KAGUYA_PHASEFUNCTION_H
#define KAGUYA_PHASEFUNCTION_H

#include <kaguya/core/Core.h>
#include <kaguya/sampler/Sampler.h>

namespace kaguya {
    namespace core {

        using kaguya::sampler::Sampler;

        class PhaseFunction {
        public:
            /**
             * p(w_o, w_i)
             * @param wo
             * @param wi
             * @return
             */
            virtual Float scatterPdf(const Vector3F &wo, const Vector3F &wi) const = 0;

            /**
             *
             * @param wo
             * @param wi
             * @return
             */
            virtual Float sampleScatter(const Vector3F &wo, Vector3F *wi, Sampler *sampler) const = 0;
        };

    }
}

#endif //KAGUYA_PHASEFUNCTION_H
