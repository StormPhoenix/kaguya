//
// Created by Storm Phoenix on 2020/10/11.
//

#ifndef KAGUYA_BDPATHTRACER_H
#define KAGUYA_BDPATHTRACER_H

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/core/Interaction.h>
#include <kaguya/core/bsdf/BSDF.h>
#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/utils/MemoryArena.h>
#include <kaguya/material/Material.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/tracer/Camera.h>
#include <kaguya/tracer/Tracer.h>
#include <kaguya/math/Math.hpp>

namespace kaguya {
    namespace tracer {

        using kaguya::core::BSDF;
        using kaguya::core::BXDFType;
        using kaguya::core::Spectrum;
        using kaguya::core::Interaction;
        using kaguya::core::SurfaceInteraction;
        using kaguya::core::VolumeInteraction;
        using kaguya::core::StartEndInteraction;
        using kaguya::material::Material;
        using kaguya::scene::Scene;
        using kaguya::tracer::Camera;
        using kaguya::memory::MemoryArena;

        typedef enum PathVertexType {
            CAMERA, LIGHT, SURFACE, VOLUME
        } PathVertexType;

        typedef struct PathVertex {
            // f(p, wo, wi) * cosine / p(w_i)
            Spectrum beta;
            // 路径点类型
            PathVertexType type;
            // 交互点
            struct {
                SurfaceInteraction si;
                VolumeInteraction vi;
                StartEndInteraction ei;
            };

            PathVertex(const SurfaceInteraction &si, const Spectrum &beta) :
                    si(si), beta(beta), type(PathVertexType::SURFACE) {}

            PathVertex(const VolumeInteraction &vi, const Spectrum &beta) :
                    vi(vi), beta(beta), type(PathVertexType::VOLUME) {}

            PathVertex(PathVertexType type, const StartEndInteraction &ei, const Spectrum &beta) :
                    type(type), ei(ei), beta(beta) {}

            // 是否可以与其他点做连接
            bool isConnectible();

            static inline PathVertex createCameraVertex(Camera *camera) {
                StartEndInteraction ei = StartEndInteraction(camera);
                return PathVertex(PathVertexType::CAMERA, ei, Spectrum(1.0));
            }

            static inline PathVertex createSurfaceVertex(const SurfaceInteraction &si, const Spectrum &beta) {
                return PathVertex(si, beta);
            }

        } PathVertex;


        /**
         * Bidirectional Path Tracing
         */
        class BDPathTracer : public Tracer {
        public:
        private:
            /**
             * 计算从相机位置出发的路径，统计每一条路径的概率密度
             *
             * 计算路径概率方法是 area product measure（路径中每一点的概率密度，probability per unit area）
             * PBRT 中需要计算的步骤包括 pdfForward 和 pdfBackward
             * @return
             */
            int generateCameraPath(Scene &scene, const Ray &ray, const Camera &camera,
                                   PathVertex *cameraSubPath, int maxDepth,
                                   MemoryArena &memoryArena) {

                assert(cameraSubPath != nullptr);
                // 第一个点 Camera
                PathVertex cameraVertex = PathVertex::createCameraVertex(camera);
                cameraSubPath[0] = cameraVertex;
                // 初始 beta
                double beta = 1.0;
                // 初始射线
                Ray scatterRay = ray;
                // 随机采样 Interaction
                for (int depth = 1; depth < maxDepth; depth++) {
                    SurfaceInteraction interaction;
                    bool isIntersected = scene.hit(scatterRay, interaction);
                    if (isIntersected) {
                        std::shared_ptr<Material> material = interaction.material;
                        assert(material != nullptr);

                        // 添加新点 TODO 默认只有 Surface 类型
                        cameraSubPath[depth] = PathVertex::createSurfaceVertex(interaction, beta);

                        BSDF *bsdf = material->bsdf(interaction, memoryArena);
                        assert(bsdf != nullptr);

                        Vector3 worldWo = -interaction.direction;
                        Vector3 worldWi = Vector3(0);
                        double samplePdf = 0;
                        // 采样下一个射线
                        Spectrum f = bsdf->sampleF(worldWo, &worldWi, &samplePdf, BXDFType::BSDF_ALL);
                        // 设置新的散射光线
                        scatterRay.setOrigin(interaction.point);
                        scatterRay.setDirection(worldWi);
                    } else {
                        break;
                    }
                }
            }

            int generateLightPath();

        private:
            std::shared_ptr<Camera> _camera = nullptr;
        };

    }
}

#endif //KAGUYA_BDPATHTRACER_H
