#include <iostream>
#include <kaguya/math/Math.hpp>
//#include <kaguya/tracer/Camera.h>
#include <kaguya/Config.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/scene/meta/Vertex.h>
#include <kaguya/tracer/PathTracer.h>

//#include <kaguya/math/Math.hpp>
//#include <kaguya/scene/meta/Sphere.h>

//using namespace kaguya::scene;

#include <kaguya/utils/ObjLoader.h>
#include <kaguya/math/RefractSampler.h>

int main() {
    using kaguya::tracer::PathTracer;
    using kaguya::tracer::Camera;
    using kaguya::tracer::Ray;
    using kaguya::Config;
    using kaguya::scene::Scene;
    using kaguya::math::RefractSampler;
    using kaguya::utils::ObjLoader;
    using kaguya::scene::Vertex;

    PathTracer pathTracer = PathTracer();
    pathTracer.run();

//    std::vector<Vertex> vertexes = ObjLoader::loadModel("./resource/objects/bunny.obj");

    return 0;
}