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
        static std::shared_ptr<Camera> buildCamera() {
            // TODO load config file
            return std::make_shared<Camera>();
        }

        // TODO 测试
        static std::shared_ptr<Scene> testBuildTwoScene() {
            std::shared_ptr<Scene> scene = std::make_shared<Scene>();
//            scene->testBuildTwoSphere();
            scene->testBuildCornelBox();
            return scene;
        }
    };

}

#endif //KAGUYA_CONFIG_H
