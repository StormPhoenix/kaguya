//
// Created by Storm Phoenix on 2021/1/7.
//

#include <kaguya/sampler/Sampler.h>

namespace kaguya {
    namespace sampler {

        Sampler::Sampler(int nSamples) :
                nSamples(nSamples),
                randomSeed(0) {}

        Sampler::Sampler(int nSamples, int seed) :
                nSamples(nSamples), randomSeed(seed) {}

        void Sampler::forPixel(const Point2I pixel) {
            currentPixel = pixel;
            sampleIndex = 0;
        }

        void Sampler::setSampleIndex(int sampleIndex) {
            sampleIndex = sampleIndex;
        }

        bool Sampler::nextSampleRound() {
            sampleIndex++;
            return sampleIndex < nSamples;
        }
    }
}