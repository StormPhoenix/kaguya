//
// Created by Storm Phoenix on 2021/4/13.
//

#ifndef KAGUYA_SIMPLEHALTONSAMPLER_H
#define KAGUYA_SIMPLEHALTONSAMPLER_H

#include <kaguya/sampler/Sampler.h>

namespace RENDER_NAMESPACE {
    namespace sampler {

        class SimpleHaltonSampler {
        public:
            RENDER_CPU_GPU
            void forPixel(const Point2I pixel) {}

            RENDER_CPU_GPU
            bool nextSampleRound();

            RENDER_CPU_GPU
            void setSampleIndex(int sampleIndex);

            RENDER_CPU_GPU
            Float sample1D();

            RENDER_CPU_GPU
            Vector2F sample2D();

            static SimpleHaltonSampler *newInstance(int nSamples, MemoryAllocator &allocator);

            SimpleHaltonSampler(int nSamples = -1);

        private:
            const int _nSamples;
            int _haltonDimension;
            int _haltonIndex;
        };

    }
}

#endif //KAGUYA_SIMPLEHALTONSAMPLER_H
