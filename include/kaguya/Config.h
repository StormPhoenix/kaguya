//
// Created by Storm Phoenix on 2020/10/1.
//

#ifndef KAGUYA_CONFIG_H
#define KAGUYA_CONFIG_H

#include <kaguya/scene/Scene.h>
#include <kaguya/scene/SimpleScene.h>
#include <kaguya/utils/memory/MemoryAllocator.h>

#include <cstring>
#include <functional>
#include <memory>
#include <vector>

namespace RENDER_NAMESPACE {
    using scene::Scene;
    using scene::SimpleScene;
    using memory::MemoryAllocator;

    class Config {
    public:

        class Tracer {
        public:
            static std::string type;
            static int maxDepth;
            static int sampleNum;
            static int delta;
            static bool strictNormals;
            static int photonCount;
            static float initialRadius;
            static float radiusDecay;
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
            // parallel tile size
            static int tileSize;
        };

        class TraceRecorder {
        public:
            static float drawPathRadius;
        };

        static std::string filenamePrefix;
        // russian roulette 从什么 depth 开始
        static int russianRouletteDepth;

        static int writeFrequency;

        // 终止概率
        static Float russianRoulette;
        // 对光源采样概率
        static Float sampleLightProb;
        // Sampler type
        static std::string samplerType;
        // Render type
        static std::string renderType;
        // GPU options
        static bool usingGPU;

        // TODO 临时代码，用于测试构建场景
        static std::shared_ptr<Scene> nextScene(MemoryAllocator &allocator);

        static std::vector<std::string> inputSceneDirs;

        static std::vector<std::function<std::shared_ptr<SimpleScene>(MemoryAllocator &)>> innerScenes;
    private:
        // Scene id
        static int sceneId;
    };

}

#endif //KAGUYA_CONFIG_H
