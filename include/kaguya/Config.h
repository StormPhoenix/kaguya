//
// Created by Storm Phoenix on 2020/10/1.
//

#ifndef KAGUYA_CONFIG_H
#define KAGUYA_CONFIG_H

#include <kaguya/scene/Hittable.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/tracer/Camera.h>
#include <memory>

namespace kaguya {

    using kaguya::tracer::Camera;
    using kaguya::scene::Hittable;
    using kaguya::scene::Scene;

    class Config {
    public:
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
        static std::shared_ptr<Scene> testBuildTwoScene() {
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();
            scene->testBuildTwoSphere();
            return scene;
        }

        // TODO 测试
        static std::shared_ptr<Scene> testBuildScene() {
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();
            scene->testBuildCornelBoxWithBunny();
//            scene->testBuildCornelBox();
            return scene;
        }
    };

}

#endif //KAGUYA_CONFIG_H
