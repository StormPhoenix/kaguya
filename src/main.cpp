#include <kaguya/common.h>
#include <kaguya/Config.h>
#include <kaguya/tracer/TracerFactory.h>

#include <iostream>
#include <ext/clipp.h>

using namespace kaguya;
using namespace RENDER_NAMESPACE::tracer;

using namespace clipp;
using namespace std;

int main(int argc, char *argv[]) {
    std::string sceneDir = "";
    auto cli = (
            value("output name", Config::filenamePrefix),

                    /* For input scene */
                    option("-scene", "--scene-directory") & value("input scene directory", sceneDir),

                    /* For general settings */
                    option("-rt", "--render-type") & value("render type", Config::renderType),
                    option("-st", "--sampler-type") & value("sampler type", Config::samplerType),
                    option("-ssp", "--sample-per-pixel") & value("sample per pixel", Config::Tracer::sampleNum),
                    option("-d", "--max-depth") & value("max scatter depth", Config::Tracer::maxDepth),
                    option("-rb", "--russian-prob") & value("russian roulette probability", Config::russianRoulette),
                    option("-rd", "--russian-depth") & value("russian roulette depth", Config::russianRouletteDepth),
                    option("-kn", "--kernel") & value("rendering kernel count", Config::Parallel::kernelCount),
                    option("-wf", "--write-frequency") & value("write frequency", Config::writeFrequency),

                    /* For stochastic progressive photon mapping settings */
                    option("-sr", "--initial-search-radius") &
                    value("initial search radius", Config::Tracer::initialRadius),
                    option("-srd", "--search-radius-decay") & value("search radius decay", Config::Tracer::radiusDecay),
                    option("-pc", "--photon-count") & value("photon count", Config::Tracer::photonCount),

                    /* Outputs settings */
                    option("-h", "--height") & value("image height", Config::Camera::height),
                    option("-w", "--width") & value("image width", Config::Camera::width)
    );

    if (!parse(argc, argv, cli)) {
        cout << make_man_page(cli, argv[0]);
        return 0;
    }

    if (sceneDir != "") {
        Config::inputSceneDirs.push_back(sceneDir);
    }
#ifdef _RENDER_GPU_MODE_
    else {
        Config::innerScenes.push_back(Scene::innerSceneWithAreaLight);
        Config::innerScenes.push_back(Scene::innerSceneBunnyWithPointLight);
    }
#endif // _RENDER_GPU_MODE_

    if (Config::Parallel::tileSize <= 0) {
        Config::Parallel::tileSize = 50;
    }

    Tracer *tracer = TracerFactory::newTracer();
    if (tracer != nullptr) {
        tracer->run();
        delete tracer;
    }

    return 0;
}