//
// Created by Storm Phoenix on 2020/10/10.
//

#include <kaguya/Config.h>

namespace kaguya {
    // 初始化场景
    int Config::sceneId = 0;
    bool Config::isScenePrepared = false;
    std::vector<std::function<std::shared_ptr<Scene>()>> Config::scenes;

    std::string Config::filenamePrefix = "render";
    std::string Config::filenameSufix = "";

    int Config::russianRouletteDepth = 3;
    Float Config::russianRoulette = 0.135;
    Float Config::sampleLightProb = 0.3f;
    int Config::kernelCount = 1;
    int Config::maxBounce = 15;
    int Config::samplePerPixel = 10;

    int Config::resolutionWidth = 100;
    int Config::resolutionHeight = 100;
    int Config::tileSize = 50;

    Float Config::initialSearchRadius = 1.0f;
    Float Config::searchRadiusDecay = 2.0 / 3.0;
    int Config::photonPerIteration = 4096;

    std::string Config::samplerType = "default";

    std::shared_ptr<Scene> Config::nextScene() {
        // 若场景未构建，则线构建场景
        if (!isScenePrepared) {
            scenes.push_back(Scene::sceneTwoBox);
//            scenes.push_back(Scene::sceneTwoSpheresWithPointLight);
//            scenes.push_back(Scene::sceneTwoSpheresWithAreaLight);
//            scenes.push_back(Scene::sceneTwoSpheresWithSpotLight);
//            scenes.push_back(Scene::sceneBunnyWithPointLight);
//            scenes.push_back(Scene::sceneBunnySubsurfaceWithAreaLight);
//            for (int i = 0; i < 200; i++) {
//                scenes.push_back(Scene::sceneBunnySubsurfaceWithAreaLightByTime);
//            }
//            scenes.push_back(Scene::sceneBunnyWithAreaLight);
//            scenes.push_back(Scene::sceneSmoke);
//                scenes.push_back(Scene::sceneLightThroughAir);
//                scenes.push_back(Scene::cornelBoxWater);
            isScenePrepared = true;
        }

        // 选取下一个场景
        if (sceneId < scenes.size()) {
            std::shared_ptr<Scene> scene = scenes[sceneId]();
            sceneId++;
            return scene;
        } else {
            return nullptr;
        }
    }
}