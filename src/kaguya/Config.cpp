//
// Created by Storm Phoenix on 2020/10/10.
//

#include <kaguya/common.h>
#include <kaguya/Config.h>
#include <kaguya/scene/importer/xml/XmlSceneImporter.h>

namespace RENDER_NAMESPACE {

    // Camera settings
    std::string Config::Camera::type = "perspective";
    std::string Config::Camera::filename = "kaguya";
    std::string Config::Camera::filmType = "ldrfilm";
    int Config::Camera::width = 1024;
    int Config::Camera::height = 1024;
    std::string Config::Camera::fileFormat = "png";
    std::string Config::Camera::pixelFormat = "rgb";
    float Config::Camera::gamma = 0.;
    bool Config::Camera::banner = false;
    std::string Config::Camera::rfilter = "tent";

    // Sampler settings
    std::string Config::Sampler::type = "independent";
    int Config::Sampler::sampleCount = 100;
    std::string Config::samplerType = "default";

    // Scene config
    int Config::sceneId = -1;
    std::vector<std::function<std::shared_ptr<SimpleScene>(MemoryAllocator &)>> Config::innerScenes;
    std::vector<std::string> Config::inputSceneDirs;

    // Trace recorder config
    float Config::TraceRecorder::drawPathRadius = 0.6;

    // Outputs settings
    std::string Config::filenamePrefix = "render";

    // Tracer settings
    int Config::russianRouletteDepth = 3;
    Float Config::russianRoulette = 0.135;
    Float Config::sampleLightProb = 0.3f;
    std::string Config::Tracer::type = "path";
    int Config::Tracer::delta = 10;
    int Config::Tracer::maxDepth = 15;
    int Config::Tracer::sampleNum = 10;
    bool Config::Tracer::strictNormals = false;
    Float Config::Tracer::initialRadius = 0.5f;
    Float Config::Tracer::radiusDecay = 2.0 / 3.0;
    int Config::Tracer::photonCount = 4096;

    std::string Config::renderType = "pt";
    int Config::writeFrequency = 20;

    // Parallel settings
    int Config::Parallel::tileSize = 50;
    int Config::Parallel::kernelCount = 1;

    bool Config::usingGPU = false;

    std::shared_ptr<Scene> Config::nextScene(MemoryAllocator &allocator) {
#ifdef _RENDER_GPU_MODE_
        if (usingGPU) {

        } else
#endif // _RENDER_GPU_MODE_
        {
            sceneId++;
            std::shared_ptr<Scene> scene = nullptr;
            if (inputSceneDirs.size() > 0) {
                using namespace scene::importer;
                XmlSceneImporter importer = XmlSceneImporter(allocator);
                if (sceneId < inputSceneDirs.size()) {
                    std::string scene_dir = fs::current_path().generic_u8string() + inputSceneDirs[sceneId];
                    scene = importer.importScene(scene_dir);
                    std::cout << "Rendering scene: " << scene->getName() << std::endl;
                    return scene;
                } else {
                    return nullptr;
                }
            } else {
                // Rendering inner scene
                if (sceneId < innerScenes.size()) {
                    scene = innerScenes[sceneId](allocator);
                    std::cout << "Rendering inner scene: " << scene->getName() << std::endl;
                    return scene;
                } else {
                    return nullptr;
                }
            }
        }
    }
}