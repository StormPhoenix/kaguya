//
// Created by Storm Phoenix on 2020/11/18.
//

#ifndef KAGUYA_SAMPLER_H
#define KAGUYA_SAMPLER_H

#include <functional>
#include <random>
#include <kaguya/math/Math.h>

namespace kaguya {
    namespace sampler {

        class Sampler {
        public:
            typedef std::shared_ptr<Sampler> Ptr;

            Sampler(int nSamples);

            Sampler(int nSamples, int seed);

            /**
             * Set current sampling pixel
             * @param pixel
             */
            virtual void forPixel(const Point2I pixel);

            virtual void setSampleIndex(int sampleIndex);

            /**
             * Begin next sample round
             * @return
             */
            virtual bool nextSampleRound();

            virtual Float sample1D() = 0;

            virtual Vector2F sample2D() = 0;

            virtual ~Sampler() {};

        protected:
            // Current pixel on which sampling
            Point2I currentPixel;
            // Sample times
            const int nSamples;
            // Random number seed
            int randomSeed = 0;
            // Sample index
            int sampleIndex;
        };
    }
}

#endif //KAGUYA_SAMPLER_H
