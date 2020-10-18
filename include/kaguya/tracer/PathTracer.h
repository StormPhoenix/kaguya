//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_PATHTRACER_H
#define KAGUYA_PATHTRACER_H

#include <memory>

#include <kaguya/scene/Scene.h>
#include <kaguya/tracer/Tracer.h>
#include <kaguya/scene/Camera.h>

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace kaguya {
    namespace tracer {

        using namespace kaguya::core;
        using kaguya::scene::Scene;

        class PathTracer : public Tracer {
        public:
            PathTracer();

            virtual void run() override;

        private:
            void init();

            /**
             * 对光源采样
             * @param scene 场景
             * @param sampleObject 从 sampleObject 发射采样光线
             * @param sampleLightRay 采样光线
             * @param sampleLightPdf 采样光线概率
             * @return 是否采样成功
             */
            bool sampleFromLights(Scene &scene, Vector3 sampleObject, Ray &sampleLightRay, double &sampleLightPdf);

            /**
             * 具体渲染代码，CPU版本
             * @param ray 射线
             * @param scene 场景
             * @param depth 反射次数
             * @return 渲染结果
             */
            Spectrum shader(const Ray &ray, Scene &scene, int depth);

            Spectrum shader2(const Ray &ray, Scene &scene, int depth);

            /**
             * 获取背景颜色，这里可以用来设置背景贴图
             * @param ray
             * @return
             */
            Spectrum background(const Ray &ray);

            /**
             * 输出渲染结果
             * @param color
             */
            void writeShaderColor(const Spectrum &spectrum, int row, int col);

            /**
             * 渲染结果写入 buffer
             * @param color
             * @param row
             * @param col
             */
            void writeBuffer(const Vector3 &color, int row, int col);

        private:
            // 相机
            std::shared_ptr<Camera> _camera = nullptr;
            // 场景
            std::shared_ptr<Scene> _scene = nullptr;
            // TODO 以下参数写入配置文件
            // 最大采样深度
            int _maxDepth = 100;
            // 每个像素采样次数
            int _samplePerPixel = 300;
            // 对光源采样概率
            double _sampleLightProb = 0.2;
            // 渲染结果位图
            int *_bitmap = nullptr;
        };
    }
}

#endif //KAGUYA_PATHTRACER_H
