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

            virtual double scatterPdf(const Vector3d &wo, const Vector3d &wi) const override;

            virtual double sampleScatter(const Vector3d &wo, Vector3d *wi, Sampler *sampler) const override;

        private:
            double henyeyGreensteinPdf(double cosTheta, double g) const;

        private:
            double _g;
        };

    }
}

#endif //KAGUYA_HENYEYGREENSTEINFUNCTION_H
