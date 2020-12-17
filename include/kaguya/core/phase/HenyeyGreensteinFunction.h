//
// Created by Storm Phoenix on 2020/11/20.
//

#ifndef KAGUYA_HENYEYGREENSTEINFUNCTION_H
#define KAGUYA_HENYEYGREENSTEINFUNCTION_H

#include <kaguya/core/phase/PhaseFunction.h>

namespace kaguya {
    namespace core {

        class HenyeyGreensteinFunction : public PhaseFunction {
        public:
            HenyeyGreensteinFunction(double g);

            virtual double scatterPdf(const Vector3 &wo, const Vector3 &wi) const override;

            virtual double sampleScatter(const Vector3 &wo, Vector3 *wi, const Sampler1D *sampler1D) const override;

        private:
            double henyeyGreensteinPdf(double cosTheta, double g) const;

        private:
            double _g;
        };

    }
}

#endif //KAGUYA_HENYEYGREENSTEINFUNCTION_H
