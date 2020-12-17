//
// Created by Storm Phoenix on 2020/11/20.
//

#ifndef KAGUYA_PHASEFUNCTION_H
#define KAGUYA_PHASEFUNCTION_H

#include <kaguya/core/Core.h>
#include <kaguya/math/Sampler.hpp>

namespace kaguya {
    namespace core {

        using kaguya::math::random::Sampler1D;

        class PhaseFunction {
        public:
            /**
             * p(w_o, w_i)
             * @param wo
             * @param wi
             * @return
             */
            virtual double scatterPdf(const Vector3 &wo, const Vector3 &wi) const = 0;

            /**
             *
             * @param wo
             * @param wi
             * @return
             */
            virtual double sampleScatter(const Vector3 &wo, Vector3 *wi, const Sampler1D *sampler1D) const = 0;
        };

    }
}

#endif //KAGUYA_PHASEFUNCTION_H
