//
// Created by Storm Phoenix on 2020/10/1.
//

#ifndef KAGUYA_CONFIG_H
#define KAGUYA_CONFIG_H

#include <kaguya/scene/Shape.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/tracer/Camera.h>

#include <cstring>
#include <functional>
#include <memory>
#include <vector>

namespace kaguya {

    using kaguya::tracer::Camera;
    using kaguya::scene::Shape;
    using kaguya::scene::Scene;

    class Config {
    public:
        // 输出图像名称前缀
        static std::string filenamePrefix;
        // 图像名称后缀
        static std::string filenameSufix;
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


        // TODO 临时代码，用于测试构建场景
        static std::shared_ptr<Scene> nextScene() {
            // 若场景未构建，则线构建场景
            if (!isScenePrepared) {
                scenes.push_back(Scene::sceneTwoSpheresWithPointLight);
                scenes.push_back(Scene::sceneTwoSpheresWithAreaLight);
                scenes.push_back(Scene::sceneTwoSpheresWithSpotLight);
                scenes.push_back(Scene::sceneBunnyWithPointLight);
                scenes.push_back(Scene::sceneBunnyWithAreaLight);
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

    private:
        // 保存场景构建函数
        static std::vector<std::function<std::shared_ptr<Scene>()>> scenes;

        // 场景数据是否准备好
        static bool isScenePrepared;

        // 场景需要
        static int sceneId;
    };

}

#endif //KAGUYA_CONFIG_H
