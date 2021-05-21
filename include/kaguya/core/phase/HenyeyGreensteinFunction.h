//
// Created by Storm Phoenix on 2020/11/20.
//

#ifndef KAGUYA_HENYEYGREENSTEINFUNCTION_H
#define KAGUYA_HENYEYGREENSTEINFUNCTION_H

#include <kaguya/core/phase/PhaseFunction.h>

namespace RENDER_NAMESPACE {
    namespace core {

        class HenyeyGreensteinFunction : public PhaseFunction {
        public:
            HenyeyGreensteinFunction(Float g);

            virtual Float scatterPdf(const Vector3F &wo, const Vector3F &wi) const override;

            virtual Float sampleScatter(const Vector3F &wo, Vector3F *wi, Sampler *sampler) const override;

        private:
            Float henyeyGreensteinPdf(Float cosTheta, Float g) const;

        private:
            Float _g;
        };

    }
}

#endif //KAGUYA_HENYEYGREENSTEINFUNCTION_H
