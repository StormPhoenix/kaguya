//
// Created by Storm Phoenix on 2020/10/10.
//

#include <kaguya/Config.h>

namespace kaguya {
    // 每像素采样次数
    int Config::samplePerPixel = 50;
    // 对光源采样概率
    double Config::sampleLightProb = 0.3f;
    // 相机宽度分辨率
    int Config::resolutionWidth = 2000;
    // 相机高度分辨率
    int Config::resolutionHeight = 2000;
}