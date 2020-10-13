//
// Created by Storm Phoenix on 2020/9/30.
//

#ifndef KAGUYA_HEMICOSINESAMPLER_H
#define KAGUYA_HEMICOSINESAMPLER_H

#include <kaguya/math/ScatterSampler.h>

namespace kaguya {
    namespace math {

        class HemiCosineSampler : public ScatterSampler {
        public:
            Vector3 sample(const Vector3 &inDir, const Vector3 &normal, double &samplePdf) override;

            double pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir) override;
        };

    }
}

#endif //KAGUYA_HEMICOSINESAMPLER_H
