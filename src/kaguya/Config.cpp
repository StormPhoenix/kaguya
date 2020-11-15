//
// Created by Storm Phoenix on 2020/10/10.
//

#include <kaguya/Config.h>

namespace kaguya {
    std::string Config::imageFilename = "render";

    int Config::russianRouletteDepth = 3;
    double Config::russianRoulette = 0.135;
    double Config::sampleLightProb = 0.3f;

    int Config::maxScatterDepth = 15;
    int Config::samplePerPixel = 10;

    int Config::resolutionWidth = 100;
    int Config::resolutionHeight = 100;
}