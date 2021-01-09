//
// Created by Storm Phoenix on 2021/1/10.
//

#ifndef KAGUYA_SAMPLERFACTORY_HPP
#define KAGUYA_SAMPLERFACTORY_HPP

#include <kaguya/Config.h>
#include <kaguya/sampler/HaltonSampler.h>
#include <kaguya/sampler/DefaultSampler.h>

namespace kaguya {
    namespace sampler {
        class SamplerFactory {
        public:
            static Sampler* newSamplerInstance() {
                if (Config::samplerType == "halton") {
                    return HaltonSampler::newInstance();
                } else {
                    return DefaultSampler::newInstance();
                }
            }
        };
    }
}

#endif //KAGUYA_SAMPLERFACTORY_HPP
