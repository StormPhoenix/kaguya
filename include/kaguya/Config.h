//
// Created by Storm Phoenix on 2020/10/1.
//

#ifndef KAGUYA_CONFIG_H
#define KAGUYA_CONFIG_H

#include <kaguya/scene/Shape.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/tracer/Camera.h>

#include <cstring>
#include <memory>

namespace kaguya {

    using kaguya::tracer::Camera;
    using kaguya::scene::Shape;
    using kaguya::scene::Scene;

    class Config {
    public:
        // 输出图像名称
        static std::string imageFilename;
        // russian roulette 从什么 depth 开始
        static int russianRouletteDepth;
        // 终止概率
        static double russianRoulette;
        // 最大散射深度
        static int maxScatterDepth;
        // 每像素采样次数
        static int samplePerPixel;
        // 对光源采样概率
        static double sampleLightProb;
        // 相机宽度分辨率
        static int resolutionWidth;
        // 相机高度分辨率
        static int resolutionHeight;

        static std::shared_ptr<Camera> buildCamera() {
            // TODO load config file
            return std::make_shared<Camera>();
        }

        // TODO 测试
        static std::shared_ptr<Scene> buildScene() {
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();
            scene->sceneTwoSpheresWithPointLight();
            return scene;
        }
    };

}

#endif //KAGUYA_CONFIG_H
