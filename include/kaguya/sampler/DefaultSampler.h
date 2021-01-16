//
// Created by Storm Phoenix on 2020/12/27.
//

#ifndef KAGUYA_DEFAULTSAMPLER_H
#define KAGUYA_DEFAULTSAMPLER_H

#include <kaguya/sampler/Sampler.h>

namespace kaguya {
    namespace sampler {

        class DefaultSampler : public Sampler {
        public:
            DefaultSampler(int samplePerPixel);

            virtual Float sample1D() override;

            virtual Vector2F sample2D() override;

            virtual bool nextSampleRound() override;

            virtual ~DefaultSampler() override {}

            static Sampler *newInstance();

        private:
            std::uniform_real_distribution<Float> distribution;
            std::mt19937 generator;
            std::function<Float()> randGenerator;
        };
    }
}

#endif //KAGUYA_DEFAULTSAMPLER_H
