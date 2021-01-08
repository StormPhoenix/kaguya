//
// Created by Storm Phoenix on 2020/12/27.
//

#ifndef KAGUYA_DEFAULTSAMPLER_H
#define KAGUYA_DEFAULTSAMPLER_H

#include <kaguya/sampler/Sampler.h>

namespace kaguya {
    namespace math {
        namespace random {

            class DefaultSampler : public Sampler {
            public:
                DefaultSampler(int samplePerPixel);

                virtual double sample1D() override;

                virtual Vector2d sample2D() override;

                virtual bool nextSampleRound() override;

                virtual ~DefaultSampler() override {}

                static Sampler *newInstance();

            private:
                std::uniform_real_distribution<double> distribution;
                std::mt19937 generator;
                std::function<double()> randGenerator;
            };
        }
    }
}

#endif //KAGUYA_DEFAULTSAMPLER_H
