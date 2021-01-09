//
// Created by Storm Phoenix on 2021/1/7.
//

#include <kaguya/sampler/Sampler.h>

namespace kaguya {
    namespace sampler {

        Sampler::Sampler(int samplePerPixel) :
                samplePerPixel(samplePerPixel),
                randomSeed(0) {}

        void Sampler::forPixel(const Point2d pixel) {
            currentPixel = pixel;
            randomSeed = 0;
        }

        bool Sampler::nextSampleRound() {
            randomSeed++;
            return randomSeed < samplePerPixel;
        }
    }
}