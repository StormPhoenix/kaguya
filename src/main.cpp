#include <kaguya/Config.h>
#include <kaguya/tracer/TracerFactory.h>

#include <iostream>
#include <clipp.h>

using namespace kaguya;
using namespace kaguya::tracer;

using namespace clipp;
using namespace std;

int main(int argc, char *argv[]) {
    auto cli = (
            value("output name", Config::filenamePrefix),
                    option("-rt", "--render-type") & value("render type", Config::renderType),
                    option("-st", "--sampler-type") & value("sampler type", Config::samplerType),
                    option("-ssp", "--sample1d-per-pixel") & value("sample1d per pixel", Config::samplePerPixel),
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
                Config::renderType.c_str(),
                Config::resolutionWidth,
                Config::resolutionHeight);
        Config::filenameSufix = string(filenameTemplate);
    } else {
        cout << make_man_page(cli, argv[0]);
        return 0;
    }

    Tracer *tracer = TracerFactory::newTracer();
    if (tracer != nullptr) {
        cout << "using render type: " << Config::renderType << endl;
        tracer->run();
        delete tracer;
    }

    return 0;
}