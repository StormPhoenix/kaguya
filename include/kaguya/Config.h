//
// Created by Storm Phoenix on 2020/10/1.
//

#ifndef KAGUYA_CONFIG_H
#define KAGUYA_CONFIG_H

#include <kaguya/scene/meta/Shape.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/tracer/Camera.h>

#include <cstring>
#include <functional>
#include <memory>
#include <vector>

namespace kaguya {

    using kaguya::tracer::Camera;
    using kaguya::scene::meta::Shape;
    using kaguya::scene::Scene;

    class Config {
    public:
        // 输出图像名称前缀
        static std::string filenamePrefix;
        // 图像名称后缀
        static std::string filenameSufix;
        // russian roulette 从什么 depth 开始
        static int russianRouletteDepth;
        // rendering kernel count
        static int kernelCount;
        // 终止概率
        static Float russianRoulette;
        // 最大散射深度
        static int maxBounce;
        // 每像素采样次数
        static int samplePerPixel;
        // 对光源采样概率
        static Float sampleLightProb;
        // 相机宽度分辨率
        static int resolutionWidth;
        // 相机高度分辨率
        static int resolutionHeight;
        // Image tile size
        static int tileSize;
        // Sampler type
        static std::string samplerType;
        // Render type
        static std::string renderType;

        /* SPPM config */
        // 初始搜索半径
        static Float initialSearchRadius;
        // 光子收集半径衰减速率
        static Float searchRadiusDecay;
        // 每轮光子发射数量
        static int photonPerIteration;

        static std::shared_ptr<Camera> buildCamera() {
            // TODO load config file
            return std::make_shared<Camera>();
        }

        // TODO 临时代码，用于测试构建场景
        static std::shared_ptr<Scene> nextScene();

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
