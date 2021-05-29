//
// Created by Storm Phoenix on 2020/12/11.
//

#ifndef KAGUYA_PATHTRACER_H
#define KAGUYA_PATHTRACER_H

#include <memory>

#include <kaguya/core/core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

#include <kaguya/tracer/Tracer.h>
#include <kaguya/utils/memory/MemoryAllocator.h>

namespace RENDER_NAMESPACE {
    namespace tracer {

        using namespace kaguya::core;
        using namespace kaguya::sampler;
        using kaguya::memory::MemoryAllocator;

        class PathTracer : public Tracer {
        public:
            PathTracer();

            virtual void render() override;

            ~PathTracer();

        private:
            virtual void init();

            /**
             * Path Tracing 渲染代码，渐进式实现
             * @param ray
             * @param scene
             * @param allocator
             * @return
             */
            Spectrum shaderOfProgression(Point2I pixelPos, int iteration, const Ray &ray,
                                         std::shared_ptr<Scene> scene, Sampler *sampler,
                                         MemoryAllocator &allocator);

            /**
             * 获取背景颜色，这里可以用来设置背景贴图
             * @param ray
             * @return
             */
            Spectrum estimateEnvironmentLights(const Ray &ray);

        private:
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
