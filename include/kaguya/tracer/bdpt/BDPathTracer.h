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
#include <kaguya/math/Math.hpp>
#include <kaguya/scene/Scene.h>
#include <kaguya/tracer/Camera.h>
#include <kaguya/tracer/Tracer.h>
#include <kaguya/tracer/bdpt/PathVertex.h>
#include <kaguya/utils/MemoryArena.h>

#include <iostream>
#include <omp.h>

namespace kaguya {
    namespace tracer {

        using kaguya::core::BSDF;
        using kaguya::core::BXDFType;
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
            virtual std::function<void(const int, const int, const Sampler1D *)> render() override;

            virtual void init();

            /**
             * 采样 ShaderColor
             * @param ray
             * @param scene
             * @param maxDepth
             * @param memoryArena
             * @return
             */
            Spectrum shader(const Ray &ray, Scene &scene, int maxDepth,
                            const Sampler1D *sampler1D, MemoryArena &memoryArena);

            Spectrum shaderOfMyImplemention();

            /**
             * 计算从相机位置出发的路径，统计每一条路径的概率密度
             *
             * 计算路径概率方法是 area product measure（路径中每一点的概率密度，probability per unit area）
             * PBRT 中需要计算的步骤包括 pdfForward 和 pdfBackward
             * @return 路径长度
             */
            int generateCameraPath(std::shared_ptr<Scene> scene, const Ray &ray, std::shared_ptr<Camera>,
                                   PathVertex *cameraSubPath, int maxDepth,
                                   const Sampler1D *sampler1D, MemoryArena &memoryArena);

            /**
             * 生成光照路径
             * @param scene
             * @param lightSubPath
             * @param maxDepth
             * @param memoryArena
             * @return
             */
            int generateLightPath(std::shared_ptr<Scene> scene,
                                  PathVertex *lightSubPath, int maxDepth,
                                  const Sampler1D *sampler1D,
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
            int randomWalk(std::shared_ptr<Scene> scene, const Ray &ray, PathVertex *path, int maxDepth,
                           double pdf, const Sampler1D *const sampler1D, MemoryArena &memoryArena, Spectrum &beta,
                           TransportMode mode);

            /**
             * 连接 Camera 和 Light 两条路径，并返回合成路径的 Radiance
             * @param scene
             * @param cameraSubPath
             * @param cameraPathLength
             * @param t cameraSubPath 参与路径构建的点数
             * @param lightSubPath
             * @param lightPathLength
             * @param s lightSubPath 参与路径构建的点数
             * @param samplePosition 最终在成像平面上采样的点。当 cameraSubPath 只去一个相机点时，samplePosition 会改变
             * @return
             */
            Spectrum connectPath(Scene &scene, PathVertex *cameraSubPath, int cameraPathLength, int t,
                                 PathVertex *lightSubPath, int lightPathLength, int s, Point2d *samplePosition,
                                 const Sampler1D *sampler1D);

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
            double misWeight(PathVertex *cameraSubPath, int t,
                             PathVertex *lightSubPath, int s,
                             PathVertex &extraVertex);

            /**
             * 计算路径中某个段中两端点段几何关系
             * @param pre
             * @param next
             * @return
             */
            Spectrum g(const PathVertex &pre, const PathVertex &next);

        private:
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
        };

    }
}

#endif //KAGUYA_BDPATHTRACER_H
