//
// Created by Storm Phoenix on 2020/10/28.
//

#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/tracer/bdpt/BDPathTracer.h>
#include <kaguya/utils/VisibilityTester.hpp>
#include <kaguya/utils/ScopeSwapper.hpp>
#include <kaguya/tracer/bdpt/PathVertex.h>


namespace kaguya {
    namespace tracer {

        using kaguya::core::BXDFType;
        using kaguya::core::TransportMode;
        using kaguya::memory::ScopeSwapper;
        using kaguya::utils::VisibilityTester;

        BDPathTracer::BDPathTracer() {
            init();
        }

        void BDPathTracer::run() {
            // TODO 临时代码；提取到 class Tracer
            if (_camera != nullptr && _scene != nullptr) {
                int cameraWidth = _camera->getResolutionWidth();
                int cameraHeight = _camera->getResolutionHeight();

                // TODO 将 bitmap 封装到写入策略模式
                // TODO 代码移动到 tracer
                _filmPlane = _camera->buildFilmPlane(SPECTRUM_CHANNEL);
                double sampleWeight = 1.0 / _samplePerPixel;
                // 已完成扫描的行数
                int finishedLine = 0;
//#pragma omp parallel for num_threads(12)
                // 遍历相机成像图案上每个像素
                for (int row = cameraHeight - 1; row >= 0; row--) {
                    MemoryArena arena;
                    for (int col = 0; col < cameraWidth; col++) {
                        MemoryArena arena;
                        Spectrum ans = {0};
                        // 做 _samplePerPixel 次采样
                        for (int sampleCount = 0; sampleCount < _samplePerPixel; sampleCount++) {
                            auto u = (col + uniformSample()) / double(cameraWidth);
                            auto v = (row + uniformSample()) / double(cameraHeight);

                            Ray sampleRay = _camera->sendRay(u, v);
                            ans += shader(sampleRay, *(_scene.get()), _maxDepth, arena);
                            // TODO 区分
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
                        std::cout << _filmPlane->getSpectrum(row, col, 0) << ' '
                                  << _filmPlane->getSpectrum(row, col, 1) << ' '
                                  << _filmPlane->getSpectrum(row, col, 2) << '\n';
                    }
                }

                delete _filmPlane;
                _filmPlane = nullptr;
            }
        }

        void BDPathTracer::init() {
            _scene = Config::testBuildScene();
            _camera = _scene->getCamera();
            _samplePerPixel = Config::samplePerPixel;
            _sampleLightProb = Config::sampleLightProb;
            _maxDepth = Config::maxScatterDepth;
            _russianRouletteBounce = Config::beginRussianRouletteBounce;
            _russianRoulette = Config::russianRoulette;
        }

        Spectrum BDPathTracer::connectPath(Scene &scene,
                                           PathVertex *cameraSubPath, int cameraPathLength, int t,
                                           PathVertex *lightSubPath, int lightPathLength, int s,
                                           Point2d *samplePosition) {
            // 检查 t 和 s 的范围，必须处于 cameraPath 和 lightPathLength 的长度范围之中
            assert(t >= 0 && t <= cameraPathLength);
            assert(s >= 0 && s <= lightPathLength);

            /* 处理策略
             *
             * Case s = 0:
             *      light 路径上没有任何点，此时 cameraSubPath 就是一个完整的路径，此时参考 cameraSubPath[t - 1] 是否发光
             *
             * Case t = 1:
             *      直接连接 camera 和 lightSubPath[s - 1]
             *
             * Case s = 1:
             *      直接对光源采样
             */

            Spectrum ret = Spectrum(0.0);

            // 额外采样的光源点
            PathVertex lightVertex;

            // 区分不同情况
            if (s == 0) {
                // lightSubPath 上没有任何路径点
                const PathVertex &pt = cameraSubPath[t - 1];
                if (pt.isLight()) {
                    ret = pt.emit(cameraSubPath[t - 2].point) * pt.beta;
                }
            } else if (t == 1) {
                // 去 lightSubPath 连接点
                const PathVertex &ps = lightSubPath[s - 1];
                if (ps.isConnectible()) {
                    VisibilityTester visibilityTester = VisibilityTester(cameraSubPath[t - 1].getInteraction(),
                                                                         ps.getInteraction());
                    if (visibilityTester.isVisible(scene)) {
                        const PathVertex &pt = cameraSubPath[0];
                        Vector3 worldWi = NORMALIZE(pt.point - ps.point);
                        // TODO check camera beta 是否正确
                        ret = ps.beta * ps.f(pt) * pt.beta;
                        if (ps.type == PathVertexType::SURFACE) {
                            ret *= ABS_DOT(ps.normal, worldWi);
                        }
                        // 由于 camera vertex 和新的点连接，需要更新 sample vertex
                        (*samplePosition) = _camera->getFilmPosition(-worldWi);
                    }
                }
            } else if (s == 1) {
                // 由于 lightSubPath 没有任何点参与路径构建，所以我们重新采样一个光源点，加到 cameraSubPath 上
                PathVertex &pt = cameraSubPath[t - 1];
                if (pt.isConnectible()) {
                    // TODO 默认只有一个光源，我们只对这个光源采样
                    std::shared_ptr<Light> light = scene.getLight();
                    Vector3 worldWi;
                    // 光源采样处的 pdf（这里指 pt 点的入射角 wi 的 pdf)
                    double sampleLightPdf;
                    VisibilityTester visibilityTester;
                    // 直接对光源采样，同时 visibilityTester 会保存两端交点
                    Spectrum sampleIntensity = light->sampleFromLight(pt.getInteraction(), &worldWi, &sampleLightPdf,
                                                                      &visibilityTester);

                    if (std::abs(sampleLightPdf - 0) < EPSILON || sampleIntensity.isBlack() ||
                        !visibilityTester.isVisible(scene)) {
                        // 没有任何光源亮度贡献的情况，直接返回 0
                        ret = Spectrum(0.0);
                    } else {
                        // 构建一个新 StartEndInteraction，用于保存路径点上的光源
                        StartEndInteraction ei = StartEndInteraction(light.get(), visibilityTester.getEnd());
                        Spectrum L = (sampleIntensity / sampleLightPdf);
                        // 由于对光源采样位置是一个新点，所以要重新创建 PathVertex
                        lightVertex =
                                PathVertex::createLightVertex(ei, L);
                        // 计算 pdfForward
                        lightVertex.pdfForward = lightVertex.computeDensityPdfOfLightOrigin(pt);

                        ret = pt.beta * pt.f(lightVertex) * lightVertex.beta;
                        if (pt.type == PathVertexType::SURFACE) {
                            ret *= std::abs(DOT(pt.normal, worldWi));
                        }
                    }
                }
            } else {
                // 其余情况
                PathVertex &pt = cameraSubPath[t - 1];
                PathVertex &ps = lightSubPath[s - 1];
                if (pt.isConnectible() && ps.isConnectible()) {
                    // 调用 pt.f() 和 ps.f，计算 pt 和 ps 连接起来的 pdf，其中 g 包含了 visible 项
                    // TODO 我不知道这里为什么没有除以 MC 采样的 pdf
                    ret = pt.beta * pt.f(ps) * ps.f(pt) * ps.beta * g(pt, ps);
                } else {
                    ret = Spectrum(0.0);
                }
            }

            double misWt = ret.isBlack() ? 0.0 : misWeight(cameraSubPath, t, lightSubPath, s, lightVertex);
            ret *= misWt;
            return ret;
        }

        int BDPathTracer::generateCameraPath(std::shared_ptr<Scene> scene, const Ray &ray,
                                             std::shared_ptr<Camera> camera, PathVertex *cameraSubPath,
                                             int maxDepth, MemoryArena &memoryArena) {
            TransportMode mode = TransportMode::RADIANCE;
            assert(cameraSubPath != nullptr);
            // 第一个点 Camera
            PathVertex cameraVertex = PathVertex::createCameraVertex(camera.get(), ray);
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
            Spectrum intensity = light->randomLightRay(&scatterRay, &lightNormal, &pdfPos, &pdfDir);
            // 创建光源点
            // TODO 没有考虑多光源情况
            lightSubPath[0] = PathVertex::createLightVertex(light.get(), scatterRay.getOrigin(),
                                                            scatterRay.getDirection(), lightNormal, intensity,
                                                            pdfPos);

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
                    Vector3 worldWo = -interaction.getDirection();
                    Vector3 worldWi = Vector3(0);
                    BXDFType sampleType;
                    Spectrum f = bsdf->sampleF(worldWo, &worldWi, &pdfPreWi, BXDFType::BSDF_ALL, &sampleType);

                    // 判断采样是否有效
                    if (std::abs(pdfPreWi) < EPSILON || f.isBlack()) {
                        break;
                    }

                    // 设置新的散射光线
                    scatterRay.setOrigin(interaction.getPoint());
                    scatterRay.setDirection(worldWi);

                    // TODO cosine 的计算感觉有问题，对于从光源发射的光线，应该用 worldWo
                    // 更新 beta
                    double cosine = std::abs(DOT(interaction.getNormal(), worldWi));
                    beta *= (f * cosine / pdfPreWi);

                    // 计算向后 pdfWo
                    // TODO 临时调换下 wo 和 wi 的位置
//                    pdfWo = bsdf->samplePdf(worldWo, worldWi);
                    pdfWo = bsdf->samplePdf(worldWi, worldWo);

                    // 如果 bsdf 反射含有 delta 成分，则前后 pdf 都赋值为 0
                    if (sampleType & BXDFType::BSDF_SPECULAR) {
                        pdfWo = 0;
                        pdfPreWi = 0;
                        vertex.isDelta = true;
                    }

                    // 更新前一个点的 pdfBackward
                    preVertex.pdfBackward = vertex.computeForwardDensityPdf(pdfWo, preVertex);
                } else {
                    break;
                }
            }
            return vertexCount;
        }

        double BDPathTracer::misWeight(PathVertex *cameraSubPath, int t,
                                       PathVertex *lightSubPath, int s,
                                       PathVertex &tempLightVertex) {
//            if (s + t == 2) {
//                return 1;
//            }
            /**
             * 1. 如果 light 有被替换，则用新的点临时替换 lightSubPath[s - 1]
             * 2. 重新计算连接位置的 cameraSubPath[t - 1].pdfBackward 和 lightSubPath[s - 1].pdfBackward
             * 3. 重新计算 cameraSubPath[t - 2].pdfBackward
             * 4. 重新计算 lightSubPath[s - 2].pdfBackward
             */

            // 提前取出 PathVertex，方便后续计算
            PathVertex *pt = t > 0 ? &cameraSubPath[t - 1] : nullptr;
            PathVertex *ptMinus = t > 1 ? &cameraSubPath[t - 2] : nullptr;

            PathVertex *ps = s > 0 ? &lightSubPath[s - 1] : nullptr;
            PathVertex *psMinus = s > 1 ? &lightSubPath[s - 2] : nullptr;

            // 当 s = 1，会对光源进行采样，此时需要更换 lightSubPath[s - 1]
            ScopeSwapper<PathVertex> swapper1;
            if (s == 1) {
                swapper1 = {&(lightSubPath[s - 1]), tempLightVertex};
            }

            // 当 t > 0， 更新 cameraSubPath[t - 1].pdfBackward
            ScopeSwapper<double> swapper2;
            if (pt != nullptr) {
                swapper2 = {&(pt->pdfBackward), s > 0 ? ps->computeDensityPdf(psMinus, *pt)
                                                      : pt->computeDensityPdfOfLightOrigin(*ptMinus)};
            }

            // 当 s > 0，更新 lightSubPath[s - 1].pdfBackward
            ScopeSwapper<double> swapper3;
            if (ps != nullptr) {
                // 这里不考虑 t 的大小，因为 t < 2 的情况是不可能进入的
                swapper3 = {&(ps->pdfBackward), pt->computeDensityPdf(ptMinus, *ps)};
            }

            // 当 t > 2，则更新 cameraSubPath[t - 2].pdfBackward
            ScopeSwapper<double> swapper4;
            if (ptMinus != nullptr) {
                swapper4 = {&(ptMinus->pdfBackward), s > 0 ? pt->computeDensityPdf(ps, *ptMinus) :
                                                     pt->computeDensityPdfFromLight(*ptMinus)};
            }

            // 当 s > 2，则更新 lightSubPath[s - 2].pdfBackward
            ScopeSwapper<double> swapper5;
            if (psMinus != nullptr) {
                swapper5 = {&(psMinus->pdfBackward), ps->computeDensityPdf(pt, *psMinus)};
            }

            // 临时替换 isDelta 项目
            ScopeSwapper<bool> swapper6;
            if (pt != nullptr) {
                swapper6 = {&(pt->isDelta), false};
            }

            ScopeSwapper<bool> swapper7;
            if (ps != nullptr) {
                swapper7 = {&(ps->isDelta), false};
            }

            // ri 项总和
            double sumRi = 0;
            // 临时 ri 项
            double ri;
            // 计算 cameraSubPath 的 Ri
            ri = 1;

            for (int i = t - 1; i >= 0; i--) {
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
            for (int i = s - 1; i >= 0; i--) {
                double pdfBackward = lightSubPath[i].pdfBackward == 0 ? 1 : lightSubPath[i].pdfBackward;
                double pdfForward = lightSubPath[i].pdfForward == 0 ? 1 : lightSubPath[i].pdfForward;
                ri *= (pdfBackward / pdfForward);

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
            return cosPre * cosNext / std::pow(dist, 2) * (visibilityTester.isVisible(*_scene) ? 1 : 0);
        }

        Spectrum BDPathTracer::shader(const Ray &ray, Scene &scene, int maxDepth, MemoryArena &memoryArena) {
            // 创建临时变量用于存储 camera、light 路径
            PathVertex *cameraSubPath = memoryArena.alloc<PathVertex>(maxDepth + 1, false);
            PathVertex *lightSubPath = memoryArena.alloc<PathVertex>(maxDepth, false);

            // 生成相机路径
            int cameraPathLength = generateCameraPath(_scene, ray, _camera, cameraSubPath, maxDepth + 1, memoryArena);

            // 生成光源路径
            int lightPathLength = generateLightPath(_scene, lightSubPath, maxDepth, memoryArena);

            Spectrum shaderColor = Spectrum(0.0);
            // 路径连接
            for (int t = 1; t <= cameraPathLength; t++) {
                // TODO delete
//            int t = 1;
                for (int s = 0; s <= lightPathLength; s++) {
                    int depth = t + s - 1;
                    if (depth <= maxDepth && depth > 0) {
                        Point2d samplePosition;
                        Spectrum value = connectPath(scene, cameraSubPath, cameraPathLength, t,
                                                     lightSubPath, lightPathLength, s, &samplePosition);

                        const double INV_WEIGHT = 1.0 / _samplePerPixel;

                        if (t == 1) {
                            // 在成像平面的 samplePosition 位置加上 value
                            _filmPlane->addSpectrum(value * INV_WEIGHT, samplePosition.y, samplePosition.x);
                        } else {
                            shaderColor += value;
                        }
                    }
                }
            }
            return shaderColor;
        }

    }
}