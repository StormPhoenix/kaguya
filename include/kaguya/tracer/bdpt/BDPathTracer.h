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

namespace kaguya {
    namespace tracer {

        using kaguya::core::BSDF;
        using kaguya::core::BXDFType;
        using kaguya::core::Spectrum;
        using kaguya::core::TransportMode;
        using kaguya::material::Material;
        using kaguya::memory::MemoryArena;
        using kaguya::scene::Scene;
        using kaguya::tracer::Camera;

        /**
         * Bidirectional Path Tracing
         */
        class BDPathTracer : public Tracer {
        public:
            BDPathTracer() {
                init();
            }

            void run() override {
                // TODO 临时测试
                if (_camera != nullptr && _scene != nullptr) {
                    int cameraWidth = _camera->getResolutionWidth();
                    int cameraHeight = _camera->getResolutionHeight();

                    double sampleWeight = 1.0 / _samplePerPixel;
                    // 已完成扫描的行数
                    int finishedLine = 0;
                    // 遍历相机成像图案上每个像素
                    for (int row = cameraHeight - 1; row >= 0; row--) {
                        MemoryArena arena;
                        for (int col = 0; col < cameraWidth; col++) {
                            MemoryArena arena;
                            Spectrum ans = {0};
                            // 做 _samplePerPixel 次采样
                            for (int sampleCount = 0; sampleCount < _samplePerPixel; sampleCount++) {
                                auto u = (col + uniformSample()) / cameraWidth;
                                auto v = (row + uniformSample()) / cameraHeight;

                                Ray sampleRay = _camera->sendRay(u, v);
                                ans += shader(sampleRay, *(_scene.get()), _maxDepth, arena);
                                arena.clean();
                            }
                            ans *= sampleWeight;
                            // TODO write to image
                        }
                    }
                }
            }

            /**
             * TODO 从 PathTracer 复制过来的，后续需要修改
             */
            void init() {
                _scene = Config::testBuildScene();
                _camera = _scene->getCamera();
                _samplePerPixel = Config::samplePerPixel;
                _sampleLightProb = Config::sampleLightProb;
                _maxDepth = Config::maxScatterDepth;
                _russianRouletteBounce = Config::beginRussianRouletteBounce;
                _russianRoulette = Config::russianRoulette;
            }

        private:

            /**
             * 采样 ShaderColor
             * @param ray
             * @param scene
             * @param maxDepth
             * @param memoryArena
             * @return
             */
            Spectrum shader(const Ray &ray, Scene &scene, int maxDepth, MemoryArena &memoryArena);

            /**
             * 计算从相机位置出发的路径，统计每一条路径的概率密度
             *
             * 计算路径概率方法是 area product measure（路径中每一点的概率密度，probability per unit area）
             * PBRT 中需要计算的步骤包括 pdfForward 和 pdfBackward
             * @return 路径长度
             */
            int generateCameraPath(std::shared_ptr<Scene> scene, const Ray &ray, std::shared_ptr<Camera>,
                                   PathVertex *cameraSubPath, int maxDepth,
                                   MemoryArena &memoryArena);

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
            int randomIntersect(std::shared_ptr<Scene> scene, const Ray &ray, PathVertex *path, int maxDepth,
                                double pdf, MemoryArena &memoryArena, Spectrum &beta, TransportMode mode);

            /**
             * 连接 Camera 和 Light 两条路径，并返回合成路径的 Radiance
             * @param scene
             * @param cameraSubPath
             * @param cameraPathLength
             * @param t cameraSubPath 参与路径构建的点数
             * @param lightSubPath
             * @param lightPathLength
             * @param s lightSubPath 参与路径构建的点数
             * @return
             */
            Spectrum connectPath(Scene &scene, PathVertex *cameraSubPath, int cameraPathLength, int t,
                                 PathVertex *lightSubPath, int lightPathLength, int s);

            double mis(PathVertex *cameraSubPath, int t,
                       PathVertex *lightSubPath, int s);

            /**
             * 计算路径中某个段中两端点段几何关系
             * @param pre
             * @param next
             * @return
             */
            double g(const PathVertex &pre, const PathVertex &next);

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

#endif //KAGUYA_BDPATHTRACER_H
