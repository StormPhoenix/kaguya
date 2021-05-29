//
// Created by Storm Phoenix on 2021/1/7.
//

#include <kaguya/sampler/Sampler.h>

namespace RENDER_NAMESPACE {
    namespace sampler {

        /* TODO delete
        Sampler::Sampler(int nSamples) :
                nSamples(nSamples),
                randomSeed(0) {}

        Sampler::Sampler(int nSamples, int seed) :
                nSamples(nSamples), randomSeed(seed) {}
                */

        void Sampler::forPixel(const Point2I pixel) {
            auto forPixelFunc = [&](auto ptr) { return ptr->forPixel(pixel); };
            return proxyCall(forPixelFunc);
        }

        void Sampler::setSampleIndex(int sampleIndex) {
            auto ssiFunc = [&](auto ptr) { return ptr->setSampleIndex(sampleIndex); };
            return proxyCall(ssiFunc);
        }

        bool Sampler::nextSampleRound() {
            auto nsrFunc = [&](auto ptr) { return ptr->nextSampleRound(); };
            return proxyCall(nsrFunc);
        }

        Float Sampler::sample1D() {
            auto sample1DFunc = [&](auto ptr) { return ptr->sample1D(); };
            return proxyCall(sample1DFunc);
        }

        Vector2F Sampler::sample2D() {
            auto sample2DFunc = [&](auto ptr) { return ptr->sample2D(); };
            return proxyCall(sample2DFunc);
        }
    }
}