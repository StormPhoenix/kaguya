#include <iostream>
#include <kaguya/math/Math.hpp>
//#include <kaguya/tracer/Camera.h>
#include <kaguya/Config.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/tracer/PathTracer.h>

//#include <kaguya/math/Math.hpp>
//#include <kaguya/scene/meta/Sphere.h>

//using namespace kaguya::scene;

#include <kaguya/math/RefractPdf.h>

int main() {
    using kaguya::tracer::PathTracer;
    using kaguya::tracer::Camera;
    using kaguya::tracer::Ray;
    using kaguya::Config;
    using kaguya::scene::Scene;
    using kaguya::math::RefractPdf;

    PathTracer pathTracer = PathTracer();
    pathTracer.run();

    return 0;
}