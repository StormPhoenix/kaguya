//
// Created by Storm Phoenix on 2020/12/11.
//

#ifndef KAGUYA_PATHTRACER_H
#define KAGUYA_PATHTRACER_H

#include <memory>

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

#include <kaguya/tracer/Tracer.h>
#include <kaguya/utils/MemoryArena.h>

namespace kaguya {
    namespace tracer {

        using namespace kaguya::core;
        using kaguya::memory::MemoryArena;

        class PathTracer : public Tracer {
        public:
            PathTracer();

            virtual std::function<void(const int, const int, const int, const int, Sampler *)> render() override;

        private:
            virtual void init();

            /**
             * Path Tracing 渲染代码，渐进式实现
             * @param ray
             * @param scene
             * @param memoryArena
             * @return
             */
            Spectrum shaderOfProgression(const Ray &ray, std::shared_ptr<Scene> scene,
                                         Sampler *sampler,
                                         MemoryArena &memoryArena);

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
            void writeBuffer(const Vector3F &color, int row, int col);

        private:
            // 最大采样深度
            int _maxDepth = 100;
            // 每个像素采样次数
            int _samplePerPixel = 300;
            // 开始 Russian Roulette 时机
            int _russianRouletteBounce;
            // Russian Roulette 概率
            Float _russianRoulette;
            // 对光源采样概率
            Float _sampleLightProb = 0.2;
        };

    }
}

#endif //KAGUYA_PATHTRACER_H
