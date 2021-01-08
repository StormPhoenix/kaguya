//
// Created by Storm Phoenix on 2020/12/27.
//

#include <kaguya/Config.h>
#include <kaguya/sampler/HaltonSampler.h>

namespace kaguya {
    namespace math {
        namespace random {

            static const int MAX_TILE_RESOLUTION = 128;
            static const int START_DIMENSION = 2;

            std::vector<uint16_t> HaltonSampler::permutation;

            /**
             * a = 1 (mod n)
             * solve a', a * a' = 1 (mode n)
             */
            static uint64_t multiplicativeInverse(int64_t a, int64_t b) {
                int64_t x, y;
                extendGCD(a, b, &x, &y);
                return mod(x, b);
            }

            bool HaltonSampler::isPrimesValid = false;

            HaltonSampler::HaltonSampler(int samplePerPixel) : Sampler(samplePerPixel) {
                // First two dimension scale
                for (int i = 0; i < 2; i++) {
                    int scale = 1;
                    int digits = 0;
                    int base = (i == 0) ? 2 : 3;
                    while (scale < std::min(MAX_TILE_RESOLUTION, Config::tileSize)) {
                        scale *= base;
                        digits++;
                    }
                    firstTwoDimScale[i] = scale;
                    firstTwoDimScaleDigits[i] = digits;
                }
                sampleStride = firstTwoDimScale[0] * firstTwoDimScale[1];
                numTheoreticReciprocal[0] = multiplicativeInverse(firstTwoDimScale[1], firstTwoDimScale[0]);
                numTheoreticReciprocal[1] = multiplicativeInverse(firstTwoDimScale[0], firstTwoDimScale[1]);
            }

            void HaltonSampler::forPixel(const Point2d pixel) {
                Sampler::forPixel(pixel);
                // new seed
                seedForPixel = seedForCurrentPixel(randomSeed);
                dimension = START_DIMENSION;
            }

            bool HaltonSampler::nextSampleRound() {
                seedForPixel = seedForCurrentPixel(randomSeed + 1);
                dimension = START_DIMENSION;
                return Sampler::nextSampleRound();
            }

            static std::mutex singleInstance;

            Sampler *HaltonSampler::newInstance() {
                // TODO add locker
                if (!isPrimesValid) {
                    {
                        std::lock_guard<std::mutex> lock(singleInstance);
                        if (!isPrimesValid) {
                            permutation = math::low_discrepancy::computePermutationArray();
                            isPrimesValid = true;
                        }
                    }
                }
                return new HaltonSampler(Config::samplePerPixel);
            }

            double HaltonSampler::sample1D() {
                if (dimension >= low_discrepancy::primeArraySize) {
                    // TODO add warning
                    dimension = low_discrepancy::primeArraySize - 1;
                }

                int base = low_discrepancy::primes[dimension];
                if (dimension <= 1) {
                    dimension++;
                    return low_discrepancy::radicalReverse(base, seedForPixel);
                } else {
                    uint16_t *perm = &permutation[low_discrepancy::primeSums[dimension]];
                    dimension++;
                    return low_discrepancy::scrambledRadicalReverse(base, seedForPixel, perm);
                }
            }

            Vector2d HaltonSampler::sample2D() {
                double u = sample1D();
                double v = sample1D();
                return Vector2d(u, v);
            }

            int HaltonSampler::seedForCurrentPixel(int seed) {
                Point2i p_j = Point2i(mod(currentPixel.x, Config::tileSize),
                                      mod(currentPixel.y, Config::tileSize));
                int dimOffset = 0;
                for (int i = 0; i < 2; i++) {
                    // l_j
                    uint64_t l_j =
                            (i == 0) ? low_discrepancy::inverseRadicalReverse<2>(p_j[i], firstTwoDimScaleDigits[i]) :
                            low_discrepancy::inverseRadicalReverse<3>(p_j[i], firstTwoDimScaleDigits[i]);
                    // m_j
                    int m_j = sampleStride / firstTwoDimScale[i];
                    // mul multiplicative inverse term
                    dimOffset += l_j * m_j * numTheoreticReciprocal[i];
                }
                dimOffset %= sampleStride;
                return dimOffset + seed * sampleStride;
            }
        }
    }
}