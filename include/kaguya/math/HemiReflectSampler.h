//
// Created by Storm Phoenix on 2020/10/1.
//

#ifndef KAGUYA_HEMIREFLECTSAMPLER_H
#define KAGUYA_HEMIREFLECTSAMPLER_H

#include <kaguya/math/ScatterSampler.h>

namespace kaguya {
    namespace math {

        class HemiReflectSampler : public ScatterSampler {
        public:
            virtual double pdf(const Vector3 &inDir, const Vector3 &normal, const Vector3 &outDir) override;

            virtual Vector3 sample(const Vector3 &inDir, const Vector3 &normal, double &samplePdf) override;
        };

    }
}

#endif //KAGUYA_HEMIREFLECTSAMPLER_H
