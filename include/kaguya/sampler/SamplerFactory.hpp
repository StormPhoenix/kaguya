//
// Created by Storm Phoenix on 2021/1/10.
//

#ifndef KAGUYA_SAMPLERFACTORY_HPP
#define KAGUYA_SAMPLERFACTORY_HPP

#include <kaguya/common.h>
#include <kaguya/Config.h>
#include <kaguya/sampler/SimpleHaltonSampler.h>
#include <kaguya/sampler/HaltonSampler.h>
#include <kaguya/sampler/DefaultSampler.h>
#include <kaguya/sampler/IndependentSampler.h>

namespace kaguya {
    namespace sampler {
        class SamplerFactory {
        public:
            static Sampler *newSampler(int nSamples = -1) {
                if (nSamples <= 0) {
                    nSamples = Config::Tracer::sampleNum;
                }
                ASSERT(nSamples > 0, "nSamples <= 0.");

                if (Config::samplerType == "halton") {
                    return HaltonSampler::newInstance(nSamples);
                } else if (Config::samplerType == "independent") {
                    return IndependentSampler::newInstance(nSamples);
                } else {
                    return DefaultSampler::newInstance(nSamples);
                }
            }

            static Sampler *newSimpleHalton(int nSamples = -1) {
                if (nSamples <= 0) {
                    nSamples = Config::Tracer::sampleNum;
                }
                ASSERT(nSamples > 0, "nSamples <= 0.");
                return SimpleHaltonSampler::newInstance(nSamples);
            }

            static Sampler *newHaltonSampler(int nSamples = -1) {
                if (nSamples <= 0) {
                    nSamples = Config::Tracer::sampleNum;
                }
                ASSERT(nSamples > 0, "nSamples <= 0.");
                return HaltonSampler::newInstance(nSamples);
            }
        };
    }
}

#endif //KAGUYA_SAMPLERFACTORY_HPP
