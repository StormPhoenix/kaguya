//
// Created by Storm Phoenix on 2021/4/13.
//

#ifndef KAGUYA_SIMPLEHALTONSAMPLER_H
#define KAGUYA_SIMPLEHALTONSAMPLER_H

#include <kaguya/sampler/Sampler.h>

namespace RENDER_NAMESPACE {
    namespace sampler {

        class SimpleHaltonSampler : public Sampler {
        public:
            virtual bool nextSampleRound() override;

            virtual void setCurrentSeed(int seed) override;

            virtual Float sample1D() override;

            virtual Vector2F sample2D() override;

            virtual ~SimpleHaltonSampler() override {}

            static Sampler *newInstance(int nSamples = -1);

        private:
            SimpleHaltonSampler(int nSamples = -1);

        private:
            int _haltonDimension;
            int _haltonIndex;
        };

    }
}

#endif //KAGUYA_SIMPLEHALTONSAMPLER_H
