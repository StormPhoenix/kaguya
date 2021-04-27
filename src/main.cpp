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

                    /* For general settings */
                    option("-rt", "--render-type") & value("render type", Config::renderType),
                    option("-st", "--sampler-type") & value("sampler type", Config::samplerType),
                    option("-ssp", "--sample-per-pixel") & value("sample per pixel", Config::Tracer::sampleNum),
                    option("-d", "--max-depth") & value("max scatter depth", Config::Tracer::maxDepth),
                    option("-rb", "--russian-prob") & value("russian roulette probability", Config::russianRoulette),
                    option("-rd", "--russian-depth") & value("russian roulette depth", Config::russianRouletteDepth),
                    option("-kn", "--kernel") & value("rendering kernel count", Config::Parallel::kernelCount),

                    /* For stochastic progressive photon mapping settings */
                    option("-sr", "--initial-search-radius") & value("initial search radius", Config::initialSearchRadius),
                    option("-srd", "--search-radius-decay") & value("search radius decay", Config::searchRadiusDecay),
                    option("-pc", "--photon-count") & value("photon count", Config::photonPerIteration),

                    /* Outputs settings */
                    option("-h", "--height") & value("image height", Config::Camera::height),
                    option("-w", "--width") & value("image width", Config::Camera::width)
    );

    if (!parse(argc, argv, cli)) {
        cout << make_man_page(cli, argv[0]);
        return 0;
    }

    Tracer *tracer = TracerFactory::newTracer();
    if (tracer != nullptr) {
        cout << "Using render type: " << Config::renderType << endl;
        tracer->run();
        delete tracer;
    }

    return 0;
}