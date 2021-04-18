//
// Created by Storm Phoenix on 2020/10/10.
//

#include <kaguya/Config.h>
#include <kaguya/scene/importer/xml/XmlSceneImporter.h>

namespace kaguya {

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

    std::string Config::Sampler::type = "independent";
    int Config::Sampler::sampleCount = 100;

    std::string Config::sceneDir = "";

    // 初始化场景
    int Config::sceneId = -1;
    bool Config::isScenePrepared = false;
    std::vector<std::function<std::shared_ptr<Scene>()>> Config::scenes;

    std::string Config::filenamePrefix = "render";

    int Config::russianRouletteDepth = 3;
    Float Config::russianRoulette = 0.135;
    Float Config::sampleLightProb = 0.3f;
    int Config::Parallel::kernelCount = 1;
    int Config::Tracer::maxDepth = 15;
    int Config::Tracer::sampleNum = 10;

    int Config::tileSize = 50;

    Float Config::initialSearchRadius = 1.0f;
    Float Config::searchRadiusDecay = 2.0 / 3.0;
    int Config::photonPerIteration = 4096;

    std::string Config::samplerType = "default";
    std::string Config::renderType = "pt";

    std::shared_ptr<Scene> Config::nextScene() {
        std::vector<std::string> sceneList = {{"resource/scenes/water-caustic/"},};
        using namespace kaguya::scene::importer;
        XmlSceneImporter importer = XmlSceneImporter();
        sceneId++;
        if (sceneId < sceneList.size()) {
            std::string scene_dir = fs::current_path() / sceneList[sceneId];
            Config::sceneDir = scene_dir;
            return importer.importScene(scene_dir);
        } else {
            return nullptr;
        }

        /* TODO delete
        // 若场景未构建，则线构建场景
        if (!isScenePrepared) {
            Scene::initSceneComponents();
            scenes.push_back(Scene::sceneCornelBoxXml);
//            scenes.push_back(Scene::sceneTwoBox);
//            scenes.push_back(Scene::sceneTwoSpheresWithPointLight);
//            scenes.push_back(Scene::sceneDeskAndBunny);
//            scenes.push_back(Scene::sceneTwoSpheresWithSpotLight);
//            scenes.push_back(Scene::sceneBunnyWithPointLight);
//            scenes.push_back(Scene::sceneBunnySubsurfaceWithAreaLight);
//            for (int i = 0; i < 200; i++) {
//                scenes.push_back(Scene::sceneBunnySubsurfaceWithAreaLightByTime);
//            }
//            scenes.push_back(Scene::sceneBunnyWithAreaLight);
//            scenes.push_back(Scene::sceneSmoke);
//            scenes.push_back(Scene::sceneLightThroughAir);
//            scenes.push_back(Scene::cornelBoxWater);
            isScenePrepared = true;
        }

        sceneId++;
        // 选取下一个场景
        if (sceneId < scenes.size()) {
            std::shared_ptr<Scene> scene = scenes[sceneId]();
            return scene;
        } else {
            return nullptr;
        }
         */
    }
}