//
// Created by Storm Phoenix on 2020/10/11.
//

#ifndef KAGUYA_BDPATHTRACER_H
#define KAGUYA_BDPATHTRACER_H

#include <kaguya/Config.h>
#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/core/Interaction.h>
#include <kaguya/core/light/Light.h>
#include <kaguya/core/bsdf/BSDF.h>
#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/material/Material.h>
#include <kaguya/math/Math.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/tracer/Camera.h>
#include <kaguya/tracer/Tracer.h>
#include <kaguya/tracer/bdpt/BDPTVertex.h>
#include <kaguya/utils/MemoryArena.h>

#include <iostream>
#include <omp.h>

namespace kaguya {
    namespace tracer {

        using kaguya::core::bsdf::BSDF;
        using kaguya::core::bsdf::BXDFType;
        using kaguya::core::Spectrum;
        using kaguya::core::TransportMode;
        using kaguya::material::Material;
        using kaguya::memory::MemoryArena;
        using kaguya::tracer::Camera;

        /**
         * Bidirectional Path Tracing
         */
        class BDPathTracer : public Tracer {
        public:
            BDPathTracer();

        private:
            virtual void render() override;


            virtual void init();

            /**
             * 采样 ShaderColor
             * @param ray
             * @param scene
             * @param maxDepth
             * @param memoryArena
             * @return
             */
            Spectrum shader(const Ray &ray, std::shared_ptr<Scene> scene, int maxDepth,
                            Sampler *sampler, MemoryArena &memoryArena);

            /**
             * 计算从相机位置出发的路径，统计每一条路径的概率密度
             *
             * 计算路径概率方法是 area product measure（路径中每一点的概率密度，probability per unit area）
             * PBRT 中需要计算的步骤包括 pdfForward 和 pdfBackward
             * @return 路径长度
             */
            int generateCameraPath(std::shared_ptr<Scene> scene, const Ray &ray, std::shared_ptr<Camera>,
                                   BDPTVertex *cameraSubPath, int maxDepth,
                                   Sampler *sampler, MemoryArena &memoryArena);

            /**
             * 生成光照路径
             * @param scene
             * @param lightSubPath
             * @param maxDepth
             * @param memoryArena
             * @return
             */
            int generateLightPath(std::shared_ptr<Scene> scene,
                                  BDPTVertex *lightSubPath, int maxDepth,
                                  Sampler *sampler,
                                  MemoryArena &memoryArena);

            /**
             * 在场景中做随机采样
             * @param scene
             * @param ray
             * @param path
             * @param maxDepth
             * @param pdf
             * @param memoryArena
             * @param beta
             * @param mode
             * @return
             */
            int randomBounce(std::shared_ptr<Scene> scene, const Ray &ray, BDPTVertex *path, int maxDepth,
                             Float pdf, Sampler *const sampler, MemoryArena &memoryArena, Spectrum &beta,
                             TransportMode mode);

            /**
             * 连接 Camera 和 Light 两条路径，并返回合成路径的 Radiance
             * @param scene
             * @param cameraSubPath
             * @param nCameraVertices
             * @param t cameraSubPath 参与路径构建的点数
             * @param lightSubPath
             * @param nLightVertices
             * @param s lightSubPath 参与路径构建的点数
             * @param samplePosition 最终在成像平面上采样的点。当 cameraSubPath 只去一个相机点时，samplePosition 会改变
             * @return
             */
            Spectrum connectPath(std::shared_ptr<Scene> scene, BDPTVertex *cameraSubPath, int nCameraVertices, int t,
                                 BDPTVertex *lightSubPath, int nLightVertices, int s, Point2F *samplePosition,
                                 Sampler *sampler);

            /**
             * 计算 connect 路径的 pdf
             *
             * 这一段参考了 PBRT，自己写的时候很容易出错
             *
             * @param cameraSubPath 相机子路径
             * @param t 相机路径长度
             * @param lightSubPath 光源子路径
             * @param s 光源路径长度
             * @param extraVertex t = 1 或 s = 1 时传入的点
             * @return
             */
            Float misWeight(BDPTVertex *cameraSubPath, int t,
                            BDPTVertex *lightSubPath, int s,
                            BDPTVertex &extraVertex);

            /**
             * 计算路径中某个段中两端点段几何关系
             * @param pre
             * @param next
             * @return
             */
            Spectrum g(const BDPTVertex &pre, const BDPTVertex &next, Sampler *sampler);

        private:
            // 最大采样深度
            int _maxDepth = 100;
            // 开始 Russian Roulette 时机
            int _russianRouletteBounce;
            // Russian Roulette 概率
            Float _russianRoulette;
            // 对光源采样概率
            Float _sampleLightProb = 0.2;
        };

    }
}

#endif //KAGUYA_BDPATHTRACER_H
