//
// Created by Storm Phoenix on 2021/5/25.
//

#ifndef KAGUYA_INDEPENDENTSAMPLER_H
#define KAGUYA_INDEPENDENTSAMPLER_H

#include <kaguya/common.h>
#include <kaguya/sampler/Sampler.h>
#include <kaguya/sampler/RNG.h>

namespace RENDER_NAMESPACE {
    namespace sampler {
        class IndependentSampler : public Sampler {
        public:
            static IndependentSampler *newInstance(int nSamples);

            IndependentSampler(int nSamples, int seed = 0) : Sampler(nSamples, seed) {}

            RENDER_CPU_GPU
            virtual void forPixel(const Point2I pixel) override;

            RENDER_CPU_GPU
            virtual void setSampleIndex(int sampleIndex) override;

            RENDER_CPU_GPU
            virtual bool nextSampleRound() override;

            RENDER_CPU_GPU
            virtual Float sample1D() override;

            RENDER_CPU_GPU
            virtual Point2F sample2D() override;

        private:
            RNG rng;
        };
    }
}

#endif //KAGUYA_INDEPENDENTSAMPLER_H
