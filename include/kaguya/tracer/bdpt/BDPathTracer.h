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

                    // TODO 将 bitmap 封装到写入策略模式
                    // TODO 代码移动到 tracer
                    _bitmap = (int *) malloc(cameraHeight * cameraWidth * SPECTRUM_CHANNEL * sizeof(unsigned int));
                    double sampleWeight = 1.0 / _samplePerPixel;
                    // 已完成扫描的行数
                    int finishedLine = 0;
//#pragma omp parallel for num_threads(12)
                    // 遍历相机成像图案上每个像素
                    for (int row = cameraHeight - 1; row >= 0; row--) {
                        MemoryArena arena;
                        for (int col = 0; col < cameraWidth; col++) {

                            // TODO delete
                            if (row == 150 && col == 40) {
                                int a = 0;
                                a ++;
                            }

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
                            // TODO 写入渲染结果，用更好的方式写入
                            writeShaderColor(ans, row, col);
                        }
//#pragma omp critical
                        finishedLine++;
                        // TODO delete
                        std::cerr << "\rScanlines remaining: " << _camera->getResolutionHeight() - finishedLine << "  "
                                  << std::flush;
                    }

                    // TODO delete
                    std::cout << "P3\n" << cameraWidth << " " << cameraHeight << "\n255\n";
                    // TODO 更改成写入替换策略
                    for (int row = cameraHeight - 1; row >= 0; row--) {
                        for (int col = 0; col < cameraWidth; col++) {
                            int offset = (row * _camera->getResolutionWidth() + col) * SPECTRUM_CHANNEL;
                            // TODO 修改此处，用于适应 Spectrum
                            // Write the translated [0,255] value of each color component.
                            std::cout << *(_bitmap + offset) << ' '
                                      << *(_bitmap + offset + 1) << ' '
                                      << *(_bitmap + offset + 2) << '\n';
                        }
                    }

                    delete[] _bitmap;
                    _bitmap = nullptr;
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

            /**
             * 计算 connect 路径的 pdf
             *
             * 这一段参考了 PBRT，自己写的时候很容易出错
             *
             * @param cameraSubPath 相机子路径
             * @param t 相机路径长度
             * @param lightSubPath 光源子路径
             * @param s 光源路径长度
             * @param tempLightVertex 如果额外进行了对光源采样，则传入 lightVertex
             * @return
             */
            double misWeight(PathVertex *cameraSubPath, int t,
                             PathVertex *lightSubPath, int s,
                             PathVertex &tempLightVertex);

            /**
             * 计算路径中某个段中两端点段几何关系
             * @param pre
             * @param next
             * @return
             */
            double g(const PathVertex &pre, const PathVertex &next);

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
        };

    }
}

#endif //KAGUYA_BDPATHTRACER_H
