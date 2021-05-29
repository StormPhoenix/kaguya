//
// Created by Storm Phoenix on 2020/12/27.
//

#include <kaguya/Config.h>
#include <kaguya/sampler/HaltonSampler.h>

namespace RENDER_NAMESPACE {
    namespace sampler {

        using namespace math;

        static const int MAX_TILE_RESOLUTION = 128;

        static const int START_DIMENSION = 0;

        std::vector<uint16_t> HaltonSampler::permutation;

        /**
         * a = 1 (mod n)
         * solve a', a * a' = 1 (mode n)
         */
        static uint64_t multiplicativeInverse(int64_t a, int64_t b) {
            int64_t x, y;
            math::extendGCD(a, b, &x, &y);
            return math::mod(x, b);
        }

        bool HaltonSampler::isPrimesValid = false;

        HaltonSampler::HaltonSampler(int nSamples) : _nSamples(nSamples) {
            // First two dimension scale
            for (int i = 0; i < 2; i++) {
                int scale = 1;
                int digits = 0;
                int base = (i == 0) ? 2 : 3;
                while (scale < std::min(MAX_TILE_RESOLUTION, Config::Parallel::tileSize)) {
                    scale *= base;
                    digits++;
                }
                firstTwoDimScales[i] = scale;
                firstTwoDimScaleDigits[i] = digits;
            }
            sampleStride = firstTwoDimScales[0] * firstTwoDimScales[1];
            numTheoreticReciprocal[0] = multiplicativeInverse(firstTwoDimScales[1], firstTwoDimScales[0]);
            numTheoreticReciprocal[1] = multiplicativeInverse(firstTwoDimScales[0], firstTwoDimScales[1]);
        }

        void HaltonSampler::forPixel(const Point2I pixel) {
            _currentPixel = pixel;
            // new seed
            seedForPixel = seedForCurrentPixel(0);
            dimension = START_DIMENSION;
        }

        void HaltonSampler::setSampleIndex(int sampleIndex) {
            _sampleIndex = sampleIndex;
            seedForPixel = seedForCurrentPixel(_sampleIndex);
        }

        bool HaltonSampler::nextSampleRound() {
            bool ret = (++_sampleIndex) < _nSamples;
            seedForPixel = seedForCurrentPixel(_sampleIndex);
            dimension = START_DIMENSION;
            return ret;
        }

        static std::mutex singleInstance;

        HaltonSampler *HaltonSampler::newInstance(int nSamples, MemoryAllocator &allocator) {
            // TODO add locker
            if (!isPrimesValid) {
                {
                    std::lock_guard<std::mutex> lock(singleInstance);
                    if (!isPrimesValid) {
                        permutation = math::sampling::low_discrepancy::computePermutationArray(true);
                        isPrimesValid = true;
                    }
                }
            }
            return allocator.newObject<HaltonSampler>(nSamples);
        }

        Float HaltonSampler::sample1D() {
            if (dimension >= math::sampling::low_discrepancy::primeArraySize) {
                // TODO add warning
                dimension = math::sampling::low_discrepancy::primeArraySize - 1;
            }

            const int dim = dimension;
            dimension++;
            if (dim == 0) {
                return math::sampling::low_discrepancy::radicalReverse(dim,
                                                                       seedForPixel >> firstTwoDimScaleDigits[0]);
            } else if (dim == 1) {
                return math::sampling::low_discrepancy::radicalReverse(dim,
                                                                       seedForPixel / firstTwoDimScales[1]);
            } else {
                uint16_t *perm = &permutation[math::sampling::low_discrepancy::primeSums[dim]];
                return math::sampling::low_discrepancy::scrambledRadicalReverse(dim, seedForPixel, perm);
            }
        }

        Vector2F HaltonSampler::sample2D() {
            Float u = sample1D();
            Float v = sample1D();
            return Vector2F(u, v);
        }

        int HaltonSampler::seedForCurrentPixel(int seed) {
            if (_currentPixel.x != pixelForOffset.x ||
                _currentPixel.y != pixelForOffset.y) {
                Point2I p_j = Point2I(mod(_currentPixel[0], MAX_TILE_RESOLUTION),
                                      mod(_currentPixel[1], MAX_TILE_RESOLUTION));
                seedForPixel = 0;
                for (int i = 0; i < 2; i++) {
                    // l_j
                        uint64_t l_j =
                                (i == 0) ? math::sampling::low_discrepancy::inverseRadicalReverse<2>(p_j[i],
                                                                                                     firstTwoDimScaleDigits[i])
                                         :
                                math::sampling::low_discrepancy::inverseRadicalReverse<3>(p_j[i],
                                                                                          firstTwoDimScaleDigits[i]);
                        // m_j
                        int m_j = sampleStride / firstTwoDimScales[i];
                        // mul multiplicative inverse term
                        seedForPixel += l_j * m_j * numTheoreticReciprocal[i];
                    }
                    seedForPixel %= sampleStride;
                    pixelForOffset = _currentPixel;
                }
                return seedForPixel + seed * sampleStride;
            }
    }
}