//
// Created by Storm Phoenix on 2020/12/27.
//

#ifndef KAGUYA_HALTONSAMPLER_H
#define KAGUYA_HALTONSAMPLER_H

#include <kaguya/sampler/Sampler.h>

namespace kaguya {
    namespace sampler {

        class HaltonSampler : public Sampler {
        public:
            virtual void forPixel(const Point2F pixel) override;

            virtual bool nextSampleRound() override;

            virtual void setCurrentSeed(int seed) override;

            virtual Float sample1D() override;

            virtual Vector2F sample2D() override;

            ~HaltonSampler() {}

        private:
            HaltonSampler(int nSamples);

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
                static Sampler *newInstance(int nSamples);
            };
    }
}

#endif //KAGUYA_HALTONSAMPLER_H
