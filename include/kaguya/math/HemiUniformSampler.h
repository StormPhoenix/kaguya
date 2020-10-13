//
// Created by Storm Phoenix on 2020/9/30.
//

#ifndef KAGUYA_HEMIUNIFORMSAMPLER_H
#define KAGUYA_HEMIUNIFORMSAMPLER_H

#include <kaguya/math/ScatterSampler.h>

namespace kaguya {
    namespace math {

        class HemiUniformSampler : public ScatterSampler {
        public:
            double pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir) override;

            Vector3 sample(const Vector3 &inDir, const Vector3 &normal, double &samplePdf) override;
        };
    }
}

#endif //KAGUYA_HEMIUNIFORMSAMPLER_H
