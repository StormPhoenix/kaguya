//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_PATHTRACER_H
#define KAGUYA_PATHTRACER_H

#include <memory>

#include <kaguya/scene/Scene.h>
#include <kaguya/tracer/Tracer.h>

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

#include <kaguya/utils/MemoryArena.h>

namespace kaguya {
    namespace tracer {

        using namespace kaguya::core;
        using kaguya::scene::Scene;
        using kaguya::memory::MemoryArena;


        class PathTracer : public Tracer {
        public:
            PathTracer();

            virtual void run() override;

        private:
            void init();

            /**
             * 在 eye 处对光源采样
             * @param scene scene
             * @param eye
             * @param bsdf
             * @param sampleRay eye 处出发的射线
             * @param lightPdf 采样概率
             * @return
             */
            Spectrum evaluateDirectLight(Scene &scene, const Interaction &eye, const BSDF &bsdf);

            /**
             * 具体渲染代码，CPU版本
             * @param ray 射线
             * @param scene 场景
             * @param depth 反射次数
             * @return 渲染结果
             */
            Spectrum shaderOfRecursion(const Ray &ray, Scene &scene, int depth, MemoryArena &memoryArena);

            /**
             * Path Tracing 渲染代码，渐进式实现
             * @param ray
             * @param scene
             * @param memoryArena
             * @return
             */
            Spectrum shaderOfProgression(const Ray &ray, Scene &scene, MemoryArena &memoryArena);

            /**
             * 获取背景颜色，这里可以用来设置背景贴图
             * @param ray
             * @return
             */
            Spectrum background(const Ray &ray);

            /**
             * 渲染结果写入 buffer
             * @param color
             * @param row
             * @param col
             */
            void writeBuffer(const Vector3 &color, int row, int col);

        private:
            // 场景
            std::shared_ptr<Scene> _scene = nullptr;
            // TODO 以下参数写入配置文件
            // 最大采样深度
            int _maxDepth = 100;
            // 每个像素采样次数
            int _samplePerPixel = 300;
            // 开始 Russian Roulette 时机
            int _russianRouletteBounce;
            // Russian Roulette 概率
            double _russianRoulette;
            // 对光源采样概率
            double _sampleLightProb = 0.2;
            // 渲染结果位图
            int *_bitmap = nullptr;
        };
    }
}

#endif //KAGUYA_PATHTRACER_H