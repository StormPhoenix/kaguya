#include <kaguya/Config.h>
#include <kaguya/math/Math.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/scene/meta/Vertex.h>
#include <kaguya/tracer/TracerFactory.h>
#include <kaguya/tracer/pt/PathTracer.h>
#include <kaguya/tracer/bdpt/BDPathTracer.h>
#include <kaguya/tracer/pm/SPPMTracer.h>

#include <kaguya/scene/importer/xml/XmlSceneImporter.h>

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
                    option("-ssp", "--sample-per-pixel") & value("sample per pixel", Config::samplePerPixel),
                    option("-d", "--max-depth") & value("max scatter depth", Config::maxBounce),
                    option("-rb", "--russian-prob") & value("russian roulette probability", Config::russianRoulette),
                    option("-rd", "--russian-depth") & value("russian roulette depth", Config::russianRouletteDepth),
                    option("-kn", "--kernel") & value("rendering kernel count", Config::kernelCount),

                    /* For stochastic progressive photon mapping settings */
                    option("-sr", "--initial-search-radius") & value("initial search radius", Config::initialSearchRadius),
                    option("-srd", "--search-radius-decay") & value("search radius decay", Config::searchRadiusDecay),
                    option("-pc", "--photon-count") & value("photon count", Config::photonPerIteration),

                    /* Outputs settings */
                    option("-h", "--height") & value("image height", Config::resolutionHeight),
                    option("-w", "--width") & value("image width", Config::resolutionWidth)
    );

    if (parse(argc, argv, cli)) {
        char filenameTemplate[80];
        sprintf(filenameTemplate, "ssp=%d_max-depth=%d_render-type=%s_%dx%d.png",
                Config::samplePerPixel,
                Config::maxBounce,
                Config::renderType.c_str(),
                Config::resolutionWidth,
                Config::resolutionHeight);
        Config::filenameSufix = string(filenameTemplate);
    } else {
        cout << make_man_page(cli, argv[0]);
        return 0;
    }

    /*
     * TODO delete
    Tracer *tracer = TracerFactory::newTracer();
    if (tracer != nullptr) {
        cout << "using render type: " << Config::renderType << endl;
        tracer->run();
        delete tracer;
    }
     */

    kaguya::scene::importer::XmlSceneImporter().importScene("./resource/scenes/cornel-box/scene.xml");

    return 0;
}