//
// Created by Storm Phoenix on 2020/12/27.
//
#include <kaguya/Config.h>
#include <kaguya/sampler/DefaultSampler.h>

namespace kaguya {
    namespace sampler {

        DefaultSampler::DefaultSampler(int nSamples) : Sampler(nSamples) {
            std::random_device rd;  //Will be used to obtain a seed for the random number engine
            generator = std::mt19937(rd());
            distribution = std::uniform_real_distribution<Float>(0.0, 1.0);
            randGenerator = std::bind(distribution, generator);
        }

        Float DefaultSampler::sample1D() {
            return randGenerator();
        }

        Vector2F DefaultSampler::sample2D() {
            Float x = sample1D();
            Float y = sample1D();
            return Vector2F(x, y);
        }

        bool DefaultSampler::nextSampleRound() {
            return true;
        }

        Sampler *DefaultSampler::newInstance(int nSamples) {
            if (nSamples <= 0) {
                nSamples = Config::Tracer::sampleNum;
            }
            return new DefaultSampler(nSamples);
        }

    }
}