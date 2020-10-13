//
// Created by Storm Phoenix on 2020/10/8.
//

#ifndef KAGUYA_REFRACTSAMPLER_H
#define KAGUYA_REFRACTSAMPLER_H

#include <kaguya/math/ScatterSampler.h>

namespace kaguya {
    namespace math {

        class RefractSampler : public ScatterSampler {
        public:
            RefractSampler(double refractiveIndex);

            virtual double pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir) override;

            virtual Vector3 sample(const Vector3 &inDir, const Vector3 &normal, double &samplePdf) override;

        private:
            double _refractiveIndex;
        };

    }
}

#endif //KAGUYA_REFRACTSAMPLER_H
