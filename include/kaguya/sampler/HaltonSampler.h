//
// Created by Storm Phoenix on 2020/12/27.
//

#ifndef KAGUYA_HALTONSAMPLER_H
#define KAGUYA_HALTONSAMPLER_H

#include <kaguya/common.h>
#include <kaguya/utils/memory/MemoryAllocator.h>

namespace RENDER_NAMESPACE {
    namespace sampler {
        using memory::MemoryAllocator;

        class HaltonSampler {
        public:
            RENDER_CPU_GPU
            void forPixel(const Point2I pixel);

            RENDER_CPU_GPU
            bool nextSampleRound();

            RENDER_CPU_GPU
            void setSampleIndex(int sampleIndex);

            RENDER_CPU_GPU
            Float sample1D();

            RENDER_CPU_GPU
            Vector2F sample2D();

            HaltonSampler(int nSamples);

        private:

            /**
             * Calculate new random seed
             *
             * Reference:
             *      1. Enumerating Quasi-Monte Carlo Point Sequences in Elementary Intervals
             *      2. Chinese Remainder Theorem
             */
            int seedForCurrentPixel(int seed);

        private:
            Point2I pixelForOffset = Point2I((std::numeric_limits<int>::max)(),
                                             (std::numeric_limits<int>::max)());
            Point2I _currentPixel;
            int _sampleIndex = 0;
            const int _nSamples;
            // Sampling seed
            int seedForPixel;
            // Dimension of sampling serial
            int dimension;

            // Primes array initialized flag
            static bool isPrimesValid;

            // Halton sampling grid
            Point2I firstTwoDimScales;
            Point2I firstTwoDimScaleDigits;
            // number-theoretic reciprocal
            Point2I numTheoreticReciprocal;
            // stride
            int sampleStride;
            // Permutation array
            static std::vector<uint16_t> permutation;

        public:

            // TODO move to factory
            static HaltonSampler *newInstance(int nSamples, MemoryAllocator &allocator);
        };
    }
}

#endif //KAGUYA_HALTONSAMPLER_H
