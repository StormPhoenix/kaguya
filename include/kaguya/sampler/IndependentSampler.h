//
// Created by Storm Phoenix on 2021/5/25.
//

#ifndef KAGUYA_INDEPENDENTSAMPLER_H
#define KAGUYA_INDEPENDENTSAMPLER_H

#include <kaguya/common.h>
#include <kaguya/sampler/RNG.h>
#include <kaguya/math/Math.h>
#include <kaguya/utils/memory/MemoryAllocator.h>

namespace RENDER_NAMESPACE {
    namespace sampler {
        using memory::MemoryAllocator;

        class IndependentSampler {
        public:
            static IndependentSampler *newInstance(int nSamples, MemoryAllocator &allocator);

            IndependentSampler(int nSamples, int seed = 0) :
                    _nSamples(nSamples), _seed(seed) {}

            RENDER_CPU_GPU
            void forPixel(const Point2I pixel);

            RENDER_CPU_GPU
            void setSampleIndex(int sampleIndex);

            RENDER_CPU_GPU
            bool nextSampleRound();

            RENDER_CPU_GPU
            Float sample1D();

            RENDER_CPU_GPU
            Point2F sample2D();

        private:
            // Current pixel on which sampling
            Point2I _currentPixel;
            // Sample times
            const int _nSamples;
            // Random number seed
            int _seed = 0;
            // Sample index
            int _sampleIndex = 0;

            RNG rng;
        };
    }
}

#endif //KAGUYA_INDEPENDENTSAMPLER_H
