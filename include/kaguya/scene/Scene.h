//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_SCENE_H
#define KAGUYA_SCENE_H

#include <kaguya/scene/Hittable.h>
#include <kaguya/tracer/Camera.h>
#include <kaguya/scene/meta/Light.h>
#include <memory>

namespace kaguya {
    namespace scene {

        using kaguya::tracer::Camera;

        /**
         * 保存场景数据
         */
        class Scene {
        public:
            /**
             * 测试场景，构建两个球体
             */
            void testBuildTwoSphere();

            /**
             * 构建 Cornel box
             */
            void testBuildCornelBox();

            /**
             * 构建 Cornel box，加载 bunny 模型
             */
            void testBuildCornelBoxWithBunny();

            bool hit(const Ray &ray, HitRecord &hitRecord);

            std::shared_ptr<Hittable> getWorld() {
                return _world;
            }

            std::shared_ptr<Camera> getCamera() {
                return _camera;
            }

            const std::shared_ptr<Light> getLight() const {
                return _light;
            }

            const std::vector<std::shared_ptr<Light>> &getLights() {
                return _lights;
            }

        protected:
            // scene objects
            std::shared_ptr<Hittable> _world = nullptr;
            // camera
            std::shared_ptr<Camera> _camera = nullptr;
            // TODO 单个光源 用于测试
            std::shared_ptr<Light> _light = nullptr;
            // lights
            std::vector<std::shared_ptr<Light>> _lights;
        };

    }
}

#endif //KAGUYA_SCENE_H
