//
// Created by Storm Phoenix on 2021/4/13.
//

#include <kaguya/Config.h>
#include <kaguya/math/Math.h>
#include <kaguya/sampler/SimpleHaltonSampler.h>

namespace RENDER_NAMESPACE {
    namespace sampler {

        using namespace math;

        SimpleHaltonSampler::SimpleHaltonSampler(int nSamples) : _nSamples(nSamples) {
            _haltonDimension = 0;
            _haltonIndex = 0;
        }

        Float SimpleHaltonSampler::sample1D() {
            return sampling::low_discrepancy::radicalReverse(_haltonDimension++, _haltonIndex);
        }

        Vector2F SimpleHaltonSampler::sample2D() {
            Float u = sample1D();
            Float v = sample1D();
            return Vector2F(u, v);
        }

        bool SimpleHaltonSampler::nextSampleRound() {
            _haltonDimension = 0;
            _haltonIndex++;

            if (_haltonIndex >= _nSamples) {
                _haltonIndex = 0;
            }
            return true;
        }

        void SimpleHaltonSampler::setSampleIndex(int sampleIndex) {
            _haltonIndex = sampleIndex;
            _haltonDimension = 0;
        }

        SimpleHaltonSampler *SimpleHaltonSampler::newInstance(int nSamples, MemoryAllocator &allocator) {
            if (nSamples <= 0) {
                nSamples = Config::Tracer::sampleNum;
            }
            return allocator.newObject<SimpleHaltonSampler>(nSamples);
        }
    }
}