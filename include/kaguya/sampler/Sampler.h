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
            Sampler(int samplePerPixel);

            /**
             * Set current sampling pixel
             * @param pixel
             */
            virtual void forPixel(const Point2F pixel);

            /**
             * Begin next sample round
             * @return
             */
            virtual bool nextSampleRound();

            virtual Float sample1D() = 0;

            virtual Vector2F sample2D() = 0;

            virtual ~Sampler() {};

        protected:
            // current pixel on which sampling
            Point2I currentPixel;

            // sample times
            const int samplePerPixel;

            // random number seed
            int randomSeed = 0;
        };
    }
}

#endif //KAGUYA_SAMPLER_H
