//
// Created by Storm Phoenix on 2020/10/28.
//

#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/tracer/bdpt/BDPathTracer.h>
#include <kaguya/utils/VisibilityTester.hpp>


namespace kaguya {
    namespace tracer {

        using kaguya::core::TransportMode;
        using kaguya::utils::VisibilityTester;
        using kaguya::core::BXDFType;

        Spectrum BDPathTracer::connectPath(Scene &scene,
                                           PathVertex *cameraSubPath, int cameraPathLength, int t,
                                           PathVertex *lightSubPath, int lightPathLength, int s) {
            // 检查 t 和 s 的范围，必须处于 cameraPath 和 lightPathLength 的长度范围之中
            assert(t >= 0 && t <= cameraPathLength);
            assert(s >= 0 && s <= lightPathLength);

            // 区分不同情况
            if (t < 2) {
                // cameraSubPath 至少包括两个点：1. 相机本身位置。2. 相机发出的射线和场景d交点
                return Spectrum(0.0f);
            } else if (s == 0) {
                // 由于 lightSubPath 没有任何点参与路径构建，所以我们重新采样一个光源点，加到 cameraSubPath 上
                PathVertex pt = cameraSubPath[t - 1];
                // TODO 默认只有一个光源，我们只对这个光源采样
                std::shared_ptr<Light> light = scene.getLight();
                Vector3 worldWi;
                double sampleLightPdf;
                VisibilityTester visibilityTester;
                // 直接对光源采样，同时 visibilityTester 会保存两端交点
                Spectrum sampleIntensity = light->sampleRay(pt.getInteraction(), &worldWi, &sampleLightPdf,
                                                            &visibilityTester);

                if (std::abs(sampleLightPdf - 0) < EPSILON || sampleIntensity.isBlack() ||
                    !visibilityTester.isVisible(scene)) {
                    // 没有任何光源亮度贡献的情况，直接返回 0
                    return Spectrum(0.0);
                } else {
                    // 构建一个新 StartEndInteraction，用于保存路径点上的光源
                    StartEndInteraction ei = StartEndInteraction(light.get(), visibilityTester.getEnd());
                    Spectrum L = (sampleIntensity / sampleLightPdf);
                    PathVertex lightVertex =
                            PathVertex::createLightVertex(ei, L);
                    Spectrum ret = pt.beta * pt.f(lightVertex) * lightVertex.beta;

                    if (pt.type == PathVertexType::SURFACE) {
                        ret *= std::abs(DOT(pt.normal, worldWi));
                    }
                    // 在对光源采样对过程中，也不做 MIS，而是直接返回
                    return ret;
                }

            } else {
                // 其余情况
                PathVertex pt = cameraSubPath[t - 1];
                PathVertex ps = lightSubPath[s - 1];
                if (pt.isConnectible() && ps.isConnectible()) {
                    // 调用 pt.f() 和 ps.f，计算 pt 和 ps 连接起来的 pdf
                    // TODO 我不知道这里为什么没有除以 MC 采样的 pdf
                    Spectrum ret = pt.beta * pt.f(ps) * ps.f(pt) * ps.beta * g(pt, ps);
                    double misWeight = mis(cameraSubPath, t, lightSubPath, s);
                    return misWeight * ret;
                } else {
                    return Spectrum(0.0);
                }
            }
        }

        int BDPathTracer::generateCameraPath(std::shared_ptr<Scene> scene, const Ray &ray,
                                             std::shared_ptr<Camera> camera, PathVertex *cameraSubPath,
                                             int maxDepth, MemoryArena &memoryArena) {
            TransportMode mode = TransportMode::RADIANCE;
            assert(cameraSubPath != nullptr);
            // 第一个点 Camera
            PathVertex cameraVertex = PathVertex::createCameraVertex(camera.get());
            cameraSubPath[0] = cameraVertex;
            // 初始 beta
            Spectrum beta = Spectrum(1.0);
            // 初始射线
            Ray scatterRay = ray;

            return randomIntersect(scene, scatterRay, cameraSubPath, maxDepth, 1.0,
                                   memoryArena, beta, TransportMode::RADIANCE);
        }

        int BDPathTracer::generateLightPath(std::shared_ptr<Scene> scene,
                                            PathVertex *lightSubPath, int maxDepth,
                                            MemoryArena &memoryArena) {
            assert(lightSubPath != nullptr);
            // TODO 默认只有一个 light
            std::shared_ptr<Light> light = scene->getLight();

            // 光源位置采样概率
            double pdfPos = 0;
            // 光线发射方向采样概率
            double pdfDir = 0;
            // 光源发射光线切面的法线
            Vector3 lightNormal;
            // 光源发射光线
            Ray scatterRay;
            // 采样光源发射
            Spectrum intensity = light->sampleLightRay(&scatterRay, &lightNormal, &pdfPos, &pdfDir);
            // 创建光源点
            lightSubPath[0] = PathVertex::createLightVertex(light.get(), scatterRay.getOrigin(),
                                                            scatterRay.getDirection(), lightNormal, intensity);

            Spectrum beta = intensity * std::abs(DOT(scatterRay.getDirection(), lightNormal)) / (pdfPos * pdfDir);

            return randomIntersect(scene, scatterRay, lightSubPath, maxDepth, pdfDir, memoryArena, beta,
                                   TransportMode::IMPORTANCE);
        }

        int BDPathTracer::randomIntersect(std::shared_ptr<Scene> scene, const Ray &ray,
                                          PathVertex *path, int maxDepth, double pdf,
                                          MemoryArena &memoryArena, Spectrum &beta,
                                          TransportMode mode) {
            // 上个路径点发射射线的 pdf
            double pdfPreWi = pdf;
            double pdfWo = 0;
            Ray scatterRay = ray;
            int vertexCount = 1;
            for (int depth = 1; depth < maxDepth; depth++) {
                if (beta.isBlack()) {
                    break;
                }

                // TODO delete
                if (depth == 4) {
                    int a = 0;
                    a ++;
                }

                SurfaceInteraction interaction;
                bool isIntersected = scene->hit(scatterRay, interaction);
                if (isIntersected) {
                    // 取出当前点和上一个点
                    PathVertex &vertex = path[depth];
                    PathVertex &preVertex = path[depth - 1];

                    // 构建 BSDF
                    BSDF *bsdf = interaction.buildBSDF(memoryArena, mode);
                    assert(bsdf != nullptr);

                    // 添加新点 TODO 默认只有 Surface 类型
                    vertex = PathVertex::createSurfaceVertex(interaction, pdfPreWi, preVertex, beta);
                    vertex.beta = beta;
                    vertexCount++;

                    // 采样下一个射线
                    Vector3 worldWo = -interaction.direction;
                    Vector3 worldWi = Vector3(0);
                    BXDFType sampleType;
                    Spectrum f = bsdf->sampleF(worldWo, &worldWi, &pdfPreWi, BXDFType::BSDF_ALL, &sampleType);

                    // 判断采样是否有效
                    if (std::abs(pdfPreWi) < EPSILON || f.isBlack()) {
                        break;
                    }

                    // 设置新的散射光线
                    scatterRay.setOrigin(interaction.point);
                    scatterRay.setDirection(worldWi);

                    // 更新 beta
                    double cosine = std::abs(DOT(interaction.normal, worldWo));
                    beta *= (f * cosine / pdfPreWi);

                    // 计算向后 pdfWo
                    pdfWo = bsdf->samplePdf(worldWi, worldWo);

                    // 如果 bsdf 反射含有 delta 成分，则前后 pdf 都赋值为 0
                    if (sampleType & BXDFType::BSDF_SPECULAR) {
                        pdfWo = 0;
                        pdfPreWi = 0;
                        vertex.isDelta = true;
                    }

                    // 更新前一个点的 pdfBackward
                    preVertex.pdfBackward = vertex.computePdfForward(pdfWo, preVertex);
                } else {
                    break;
                }
            }
            return vertexCount;
        }

        double BDPathTracer::mis(PathVertex *cameraSubPath, int t,
                                 PathVertex *lightSubPath, int s) {
            // ri 项总和
            double sumRi = 0;
            // 临时 ri 项
            double ri;
            // 计算 cameraSubPath 的 Ri
            ri = 1;
            for (int i = t - 1; i > 0; i--) {
                double pdfBackward = cameraSubPath[i].pdfBackward == 0 ? 1 : cameraSubPath[i].pdfBackward;
                double pdfForward = cameraSubPath[i].pdfForward == 0 ? 1 : cameraSubPath[i].pdfForward;
                ri *= (pdfBackward / pdfForward);

                // 当前点 cameraSubPath[i] 和 cameraSubPath[i - 1] 是连接点的时候，不应该被连起来。
                if (!cameraSubPath[i].isDelta && !cameraSubPath[i - 1].isDelta) {
                    sumRi += ri;
                }
            }

            // 计算 lightSubPath 的 Ri
            ri = 1;
            for (int i = s - 1; i > 0; i--) {
                double pdfBackward = lightSubPath[i].pdfBackward == 0 ? 1 : lightSubPath[i].pdfBackward;
                double pdfForward = lightSubPath[i].pdfForward == 0 ? 1 : lightSubPath[i].pdfForward;

                // 1. 如果还没有到达最后的灯源点（i > 0），则要像 cameraSubPath 一样判断是否是 delta 分布
                // 2. 如果到达了最后的光源点 (i = 0)，则判断光源是否是 delta
                // 以上两种 delta 情况都不会进行计算 ri
                if (!lightSubPath[i].isDelta &&
                    !(i > 0 ? lightSubPath[i - 1].isDelta : lightSubPath[i].isDeltaLight())) {
                    sumRi += ri;
                }
            }
            return 1 / (sumRi + 1);
        }

        double BDPathTracer::g(const PathVertex &pre, const PathVertex &next) {
            Vector3 dirToNext = next.point - pre.point;
            double dist = LENGTH(dirToNext);
            dirToNext = NORMALIZE(dirToNext);

            double cosPre = 1.0;
            if (pre.type == PathVertexType::SURFACE) {
                cosPre = std::abs(DOT(pre.normal, dirToNext));
            }

            double cosNext = 1.0;
            if (next.type == PathVertexType::SURFACE) {
                cosNext = std::abs(DOT(next.normal, dirToNext));
            }

            VisibilityTester visibilityTester = VisibilityTester(pre.getInteraction(), next.getInteraction());
            // TODO _scene 替换成 scene
            return cosPre * cosNext / std::pow(dist, 2) * (visibilityTester.isVisible(*_scene) ? 1 : 0);
        }

        Spectrum BDPathTracer::shader(const Ray &ray, Scene &scene, int maxDepth, MemoryArena &memoryArena) {
            // 创建临时变量用于存储 camera、light 路径
            PathVertex *cameraSubPath = memoryArena.alloc<PathVertex>(maxDepth, false);
            PathVertex *lightSubPath = memoryArena.alloc<PathVertex>(maxDepth, false);

            // 生成相机路径
            int cameraPathLength = generateCameraPath(_scene, ray, _camera, cameraSubPath, maxDepth, memoryArena);

            // 生成光源路径
            int lightPathLength = generateLightPath(_scene, lightSubPath, maxDepth, memoryArena);

            Spectrum shaderColor = Spectrum(0.0);
            // 连接点
            for (int t = 2; t <= cameraPathLength; t++) {
                for (int s = 0; s <= lightPathLength; s++) {
                    shaderColor += connectPath(scene, cameraSubPath, cameraPathLength, t,
                                               lightSubPath, lightPathLength, s);
                }
            }
            return shaderColor;
        }

    }
}