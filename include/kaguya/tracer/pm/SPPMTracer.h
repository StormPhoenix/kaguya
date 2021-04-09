//
// Created by Storm Phoenix on 2021/4/8.
//

#ifndef KAGUYA_SPPMTRACER_H
#define KAGUYA_SPPMTRACER_H

#include <kaguya/tracer/Tracer.h>

namespace kaguya {
    namespace tracer {

        class SPPMTracer : public Tracer {
        private:
            virtual std::function<void(const int, const int, Sampler *)> render() override;
        };

    }
}

#endif //KAGUYA_SPPMTRACER_H
