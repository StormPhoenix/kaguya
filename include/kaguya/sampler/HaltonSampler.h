//
// Created by Storm Phoenix on 2020/12/27.
//

#ifndef KAGUYA_HALTONSAMPLER_H
#define KAGUYA_HALTONSAMPLER_H

#include <kaguya/sampler/Sampler.h>

namespace kaguya {
    namespace math {
        namespace random {

            class HaltonSampler : public Sampler {
            public:
                HaltonSampler(int samplePerPixel);

                virtual void forPixel(const Point2d pixel) override;

                virtual bool nextSampleRound() override;

                virtual double sample1D() override;

                virtual Vector2d sample2D() override;

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
                // Sampling seed
                int seedForPixel;

                // Dimension of sampling serial
                int dimension;

                // Primes array initialized flag
                static bool isPrimesValid;

                // Halton sampling grid
                Point2i firstTwoDimScale;
                Point2i firstTwoDimScaleDigits;
                // number-theoretic reciprocal
                Point2i numTheoreticReciprocal;
                // stride
                int sampleStride;
                // Permutation array
                static std::vector<uint16_t> permutation;

            public:

                // TODO move to factory
                static Sampler *newInstance();
            };
        }
    }
}

#endif //KAGUYA_HALTONSAMPLER_H
