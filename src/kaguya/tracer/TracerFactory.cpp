//
// Created by Storm Phoenix on 2021/1/20.
//

#include <kaguya/Config.h>
#include <kaguya/tracer/TracerFactory.h>
#include <kaguya/tracer/bdpt/BDPathTracer.h>
#include <kaguya/tracer/pt/PathTracer.h>

namespace kaguya {
    namespace tracer {
        Tracer *TracerFactory::newTracer() {
            Tracer *tracer = nullptr;

            std::string render = Config::renderType;
            if (render == "bdpt") {
                tracer = new BDPathTracer();
            } else if (render == "pt") {
                tracer = new PathTracer();
            } else {
                std::cout << "not support render type: " << render << std::endl;
            }
            return tracer;
        }
    }
}