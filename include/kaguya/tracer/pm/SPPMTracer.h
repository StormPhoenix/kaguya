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
            SPPMTracer(Float initialRadius = 1.0f);

        private:
            virtual std::function<void(const int, const int, const int, const int, Sampler *)> render() override;

        private:
            int _writeFrequence = 20;

            Float _gamma = 2.0 / 3.0;

            int _samplePerPixels = 300;

            int _shootPhotonsPerIter = 8192;

            int _maxDepth = 100;

            Float _initialRadius = 1.0f;

        };

    }
}

#endif //KAGUYA_SPPMTRACER_H
