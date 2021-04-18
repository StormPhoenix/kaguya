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



        class Tracer {
        public:
            static std::string type;
            static int maxDepth;
            static int sampleNum;
            static int delta;
            static bool strictNormals;
        };

        class Camera {
        public:
            static std::string type;
            static std::string filename;
            static std::string filmType;
            static int width;
            static int height;
            static std::string fileFormat;
            static std::string pixelFormat;
            static float gamma;
            static bool banner;
            static std::string rfilter;
        };

        class Sampler {
        public:
            static std::string type;
            static int sampleCount;
        };

        class Parallel {
        public:
            // rendering kernel count
            static int kernelCount;
        };

        static std::string sceneDir;
        // TODO delete
        // 输出图像名称前缀
        static std::string filenamePrefix;
        // russian roulette 从什么 depth 开始
        static int russianRouletteDepth;

        // 终止概率
        static Float russianRoulette;
        // 对光源采样概率
        static Float sampleLightProb;
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

        // TODO 临时代码，用于测试构建场景
        static std::shared_ptr<Scene> nextScene();

    private:
        // TODO delete
        // 保存场景构建函数
        static std::vector<std::function<std::shared_ptr<Scene>()>> scenes;

        // 场景数据是否准备好
        static bool isScenePrepared;

        // 场景需要
        static int sceneId;
    };

}

#endif //KAGUYA_CONFIG_H
