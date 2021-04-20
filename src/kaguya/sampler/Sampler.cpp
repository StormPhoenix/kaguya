//
// Created by Storm Phoenix on 2021/1/7.
//

#include <kaguya/sampler/Sampler.h>

namespace kaguya {
    namespace sampler {

        Sampler::Sampler(int nSamples) :
                nSamples(nSamples),
                randomSeed(0) {}

        void Sampler::forPixel(const Point2F pixel) {
            currentPixel = pixel;
            randomSeed = 0;
        }

        void Sampler::setCurrentSeed(int seed) {
            randomSeed = seed;
        }

        bool Sampler::nextSampleRound() {
            randomSeed++;
            return randomSeed < nSamples;
        }
    }
}