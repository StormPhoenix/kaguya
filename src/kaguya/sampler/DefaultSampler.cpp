//
// Created by Storm Phoenix on 2020/12/27.
//
#include <kaguya/Config.h>
#include <kaguya/sampler/DefaultSampler.h>

namespace kaguya {
    namespace sampler {

        DefaultSampler::DefaultSampler(int samplePerPixel) : Sampler(samplePerPixel) {
            std::random_device rd;  //Will be used to obtain a seed for the random number engine
            generator = std::mt19937(rd());
            distribution = std::uniform_real_distribution<double>(0.0, 1.0);
            randGenerator = std::bind(distribution, generator);
        }

        double DefaultSampler::sample1D() {
            return randGenerator();
        }

        Vector2d DefaultSampler::sample2D() {
            double x = sample1D();
            double y = sample1D();
            return Vector2d(x, y);
        }

        bool DefaultSampler::nextSampleRound() {
            return true;
        }

        Sampler *DefaultSampler::newInstance() {
            return new DefaultSampler(Config::samplePerPixel);
        }

    }
}