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
    double Config::russianRoulette = 0.135;
    double Config::sampleLightProb = 0.3f;
    int Config::kernelCount = 1;
    int Config::maxScatterDepth = 15;
    int Config::samplePerPixel = 10;

    int Config::resolutionWidth = 100;
    int Config::resolutionHeight = 100;
    int Config::tileSize = 64;

    std::string Config::samplerType = "default";
}