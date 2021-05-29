//
// Created by Storm Phoenix on 2021/1/10.
//

#ifndef KAGUYA_SAMPLERFACTORY_HPP
#define KAGUYA_SAMPLERFACTORY_HPP

#include <kaguya/common.h>
#include <kaguya/Config.h>
#include <kaguya/sampler/SimpleHaltonSampler.h>
// TODO
// #include <kaguya/sampler/HaltonSampler.h>
#include <kaguya/sampler/IndependentSampler.h>

namespace RENDER_NAMESPACE {
    namespace sampler {
        class SamplerFactory {
        public:
            static Sampler newSampler(int nSamples) {
                if (nSamples <= 0) {
                    nSamples = Config::Tracer::sampleNum;
                }
                ASSERT(nSamples > 0, "nSamples <= 0.");
                // TODO add halton sampler
                return new IndependentSampler(nSamples);
            }

            static Sampler newSampler(int nSamples, MemoryAllocator &allocator) {
                if (nSamples <= 0) {
                    nSamples = Config::Tracer::sampleNum;
                }
                ASSERT(nSamples > 0, "nSamples <= 0.");
// TODO
//                if (Config::samplerType == "halton") {
//                    return HaltonSampler::newInstance(nSamples, allocator);
//                } else {
                return IndependentSampler::newInstance(nSamples, allocator);
//                }
            }

            static SimpleHaltonSampler *newSimpleHalton(int nSamples) {
                if (nSamples <= 0) {
                    nSamples = Config::Tracer::sampleNum;
                }
                ASSERT(nSamples > 0, "nSamples <= 0.");
                return new SimpleHaltonSampler(nSamples);
            }

            static Sampler newSimpleHalton(int nSamples, MemoryAllocator &allocator) {
                if (nSamples <= 0) {
                    nSamples = Config::Tracer::sampleNum;
                }
                ASSERT(nSamples > 0, "nSamples <= 0.");
                return SimpleHaltonSampler::newInstance(nSamples, allocator);
            }
        };
    }
}

#endif //KAGUYA_SAMPLERFACTORY_HPP
