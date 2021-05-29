//
// Created by Storm Phoenix on 2020/11/18.
//

#ifndef KAGUYA_SAMPLER_H
#define KAGUYA_SAMPLER_H

#include <kaguya/common.h>
#include <kaguya/utils/TaggedPointer.h>
#include <kaguya/sampler/IndependentSampler.h>
#include <kaguya/sampler/HaltonSampler.h>
#include <kaguya/sampler/SimpleHaltonSampler.h>

namespace RENDER_NAMESPACE {
    namespace sampler {
        using memory::TaggedPointer;

        class Sampler : public TaggedPointer<IndependentSampler, SimpleHaltonSampler> {
        public:
            using TaggedPointer::TaggedPointer;

            /**
             * Set current sampling pixel
             * @param pixel
             */
            RENDER_CPU_GPU void forPixel(const Point2I pixel);

            RENDER_CPU_GPU void setSampleIndex(int sampleIndex);

            /**
             * Begin next sample round
             * @return
             */
            RENDER_CPU_GPU bool nextSampleRound();

            RENDER_CPU_GPU Float sample1D();

            RENDER_CPU_GPU Vector2F sample2D();
        };
    }
}

#endif //KAGUYA_SAMPLER_H
