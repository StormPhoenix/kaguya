//
// Created by Storm Phoenix on 2021/5/25.
//

#include <kaguya/sampler/IndependentSampler.h>
#include <kaguya/Config.h>

namespace RENDER_NAMESPACE {
    namespace sampler {
        IndependentSampler * IndependentSampler::newInstance(int nSamples) {
            if (nSamples <= 0) {
                nSamples = Config::Tracer::sampleNum;
            }
            return new IndependentSampler(nSamples);
        }

        void IndependentSampler::forPixel(const Point2I pixel) {
            Sampler::forPixel(pixel);
            rng.newSequence((pixel.x + pixel.y * 65536) | (uint64_t(randomSeed) << 32));
        }

        void IndependentSampler::setSampleIndex(int sampleIndex) {
            int px = currentPixel.x;
            int py = currentPixel.y;
            rng.newSequence((px + py * 65536) | (uint64_t(randomSeed) << 32));
            rng.advance(sampleIndex * 65536 + 0);
        }

        bool IndependentSampler::nextSampleRound() {
            bool ret = Sampler::nextSampleRound();
            rng.advance(sampleIndex * 65536 + 0);
            return ret;
        }

        Float IndependentSampler::sample1D() {
            return rng.uniform<Float>();
        }

        Point2F IndependentSampler::sample2D() {
            return {rng.uniform<Float>(), rng.uniform<Float>()};
        }
    }
}