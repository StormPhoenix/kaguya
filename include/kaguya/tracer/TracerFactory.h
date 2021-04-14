//
// Created by Storm Phoenix on 2021/1/20.
//

#ifndef KAGUYA_TRACERFACTORY_H
#define KAGUYA_TRACERFACTORY_H

#include <kaguya/tracer/Tracer.h>

namespace kaguya {
    namespace tracer {
        class TracerFactory final {
        public:
            static Tracer *newTracer();
        };
    }
}

#endif //KAGUYA_TRACERFACTORY_H
