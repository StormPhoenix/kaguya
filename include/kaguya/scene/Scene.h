//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_SCENE_H
#define KAGUYA_SCENE_H

#include <kaguya/scene/Hittable.h>
#include <kaguya/tracer/Camera.h>
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

            bool hit(const Ray &ray, HitRecord &hitRecord);

            std::shared_ptr<Hittable> getWorld() {
                return _world;
            }

            std::shared_ptr<Camera> getCamera() {
                return _camera;
            }

        protected:
            std::shared_ptr<Hittable> _world = nullptr;
            std::shared_ptr<Camera> _camera = nullptr;
        };

    }
}

#endif //KAGUYA_SCENE_H
