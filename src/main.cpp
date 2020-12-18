#include <kaguya/Config.h>
#include <kaguya/math/Math.hpp>
#include <kaguya/scene/Scene.h>
#include <kaguya/scene/meta/Vertex.h>
#include <kaguya/tracer/pt/PathTracer.h>
#include <kaguya/tracer/bdpt/BDPathTracer.h>

#include <iostream>
#include <cstring>
#include <clipp.h>

using namespace kaguya;
using namespace kaguya::tracer;

using namespace clipp;
using namespace std;

int main(int argc, char *argv[]) {

//    PathTracer pathTracer = PathTracer();
//    pathTracer.run();

    string render = "bdpt";

    auto cli = (
            value("output name", Config::filenamePrefix),
                    option("-rt", "--render-type") & value("render type", render),
                    option("-ssp", "--sample-per-pixel") & value("sample per pixel", Config::samplePerPixel),
                    option("-d", "--max-depth") & value("max scatter depth", Config::maxScatterDepth),
                    option("-rb", "--russian-prob") & value("russian roulette probability", Config::russianRoulette),
                    option("-rd", "--russian-depth") & value("russian roulette depth", Config::russianRouletteDepth),
                    option("-kn", "--kernel") & value("rendering kernel count", Config::kernelCount),
                    option("-h", "--height") & value("image height", Config::resolutionHeight),
                    option("-w", "--width") & value("image width", Config::resolutionWidth)
    );

    if (parse(argc, argv, cli)) {
        char filenameTemplate[80];
        sprintf(filenameTemplate, "ssp=%d_max-depth=%d_render-type=%s_%dx%d.png",
                Config::samplePerPixel,
                Config::maxScatterDepth,
                render.c_str(),
                Config::resolutionWidth,
                Config::resolutionHeight);
        Config::filenameSufix = string(filenameTemplate);
    } else {
        cout << make_man_page(cli, argv[0]);
        return 0;
    }

    Tracer *tracer = nullptr;

    if (render == "bdpt") {
        tracer = new BDPathTracer();
    }else if (render == "pt"){
        tracer = new PathTracer();
    } else {
        cout << "not support render type: " << render << endl;
    }

    if (tracer != nullptr) {
        cout << "using render type: " << render << endl;
        tracer->run();
        delete tracer;
    }

    return 0;
}