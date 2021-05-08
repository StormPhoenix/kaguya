//
// Created by Storm Phoenix on 2021/4/8.
//

#ifndef KAGUYA_SPPMTRACER_H
#define KAGUYA_SPPMTRACER_H

#include <kaguya/tracer/Tracer.h>

namespace kaguya {
    namespace tracer {

        class SPPMTracer : public Tracer {
        public:
            SPPMTracer();

        private:
            virtual void render() override;

        private:
            Float _gamma = 2.0 / 3.0;

            int _shootPhotonsPerIter = 8192;

            int _maxDepth = 100;

            Float _initialRadius = 1.0f;

        };

    }
}

#endif //KAGUYA_SPPMTRACER_H
