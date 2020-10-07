//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_PATHTRACER_H
#define KAGUYA_PATHTRACER_H

#include <memory>

#include <kaguya/scene/Scene.h>
#include <kaguya/tracer/Camera.h>

namespace kaguya {
    namespace tracer {

        using kaguya::scene::Scene;

        class PathTracer {
        public:
            PathTracer();

            void run();

        private:
            void init();

            /**
             * 具体渲染代码，CPU版本
             * @param ray 射线
             * @param scene 场景
             * @param depth 反射次数
             * @return 渲染结果
             */
            Vector3 shader(const Ray &ray, Scene &scene, int depth);

            /**
             * 获取背景颜色，这里可以用来设置背景贴图
             * @param ray
             * @return
             */
            Vector3 background(const Ray &ray);

            /**
             * 输出渲染结果
             * @param color
             */
            void writeShaderColor(const Vector3 &color);

        private:
            // 相机
            std::shared_ptr<Camera> _camera = nullptr;
            // 场景
            std::shared_ptr<Scene> _scene = nullptr;
            // 最大采样深度
            int _maxDepth = 50;
            // 每个像素采样次数
            int _samplePerPixel = 50;
        };
    }
}

#endif //KAGUYA_PATHTRACER_H
