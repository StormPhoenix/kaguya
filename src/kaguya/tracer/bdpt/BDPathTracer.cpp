//
// Created by Storm Phoenix on 2020/10/28.
//

#include <kaguya/Common.h>
#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/core/medium/Medium.h>
#include <kaguya/tracer/bdpt/BDPathTracer.h>
#include <kaguya/tracer/bdpt/PathVertex.h>
#include <kaguya/utils/VisibilityTester.h>
#include <kaguya/utils/ScopeSwapper.hpp>
#include <kaguya/sampler/SamplerFactory.hpp>
#include <kaguya/parallel/RenderPool.h>

#include <atomic>

namespace kaguya {
    namespace tracer {

        using kaguya::core::bsdf::BXDFType;
        using kaguya::core::medium::Medium;
        using kaguya::core::TransportMode;
        using kaguya::memory::ScopeSwapper;
        using kaguya::utils::VisibilityTester;

        BDPathTracer::BDPathTracer() {
            init();
        }

        void BDPathTracer::render() {
            ASSERT(Config::Parallel::tileSize > 0, "Tile size lower than zero. ")
            ASSERT(Config::Tracer::sampleNum > 0, "Sample number lower than zero. ")

            int width = Config::Camera::width;
            int height = Config::Camera::height;

            int nTileX = (width + Config::Parallel::tileSize - 1) / Config::Parallel::tileSize;
            int nTileY = (height + Config::Parallel::tileSize - 1) / Config::Parallel::tileSize;

            const int nIterations = Config::Tracer::sampleNum;

            std::atomic<int> nFinished(0);
            std::cout << "\r" << float(nFinished) * 100 / (nIterations) << " %" << std::flush;
            for (int iter = 0; iter < nIterations; iter++) {
                auto renderFunc = [&](const int idxTileX, const int idxTileY) -> void {
                    int startRow = idxTileY * Config::Parallel::tileSize;
                    int endRow = std::min(startRow + Config::Parallel::tileSize - 1, height - 1);

                    int startCol = idxTileX * Config::Parallel::tileSize;
                    int endCol = std::min(startCol + Config::Parallel::tileSize - 1, width - 1);

                    int tileWidth = endCol - startCol + 1;
                    int tileHeight = endRow - startRow + 1;
                    FilmTile::Ptr filmTile = std::make_shared<FilmTile>(Point2I(startCol, startRow),
                                                                        tileWidth, tileHeight);

                    MemoryArena arena;
                    Sampler *sampler = sampler::SamplerFactory::newSampler(Config::Tracer::sampleNum);
                    for (int row = startRow; row <= endRow; row++) {
                        for (int col = startCol; col <= endCol; col++) {
                            // Set current sampling pixel
                            sampler->forPixel(Point2F(row, col));
                            sampler->setCurrentSeed(iter);

                            Float pixelX = col + sampler->sample1D();
                            Float pixelY = row + sampler->sample1D();
                            Ray sampleRay = _camera->generateRay(pixelX, pixelY, sampler);

                            Spectrum shaderColor = shader(sampleRay, _scene, _maxDepth, sampler, arena);
                            arena.clean();

                            filmTile->addSpectrum(shaderColor, row - startRow, col - startCol);
                        }
                    }
                    _filmPlane->mergeTile(filmTile);
                    delete sampler;
                };
                parallel::parallelFor2D(renderFunc, Point2I(nTileX, nTileY));
                nFinished++;
                std::cout << "\r" << float(nFinished) * 100 / (nIterations) << " %" << std::flush;

                // Write image
                if ((Config::writeFrequency > 0 && (iter + 1) % Config::writeFrequency == 0) ||
                    iter == nIterations - 1) {
                    Float sampleWeight = 1.0 / (iter + 1);
                    std::string suffixSSP;
                    std::stringstream ss;
                    ss << "_SSP" << iter + 1 << "_";
                    ss >> suffixSSP;

                    _filmPlane->writeImage((Config::filenamePrefix + suffixSSP + _scene->getName()).c_str(),
                                           sampleWeight);
                }
            }
            writeImage(Config::filenamePrefix + "_" + _scene->getName(), 1.0 / nIterations);
        }

        void BDPathTracer::init() {
            _sampleLightProb = Config::sampleLightProb;
            _maxDepth = Config::Tracer::maxDepth;
            _russianRouletteBounce = Config::russianRouletteDepth;
            _russianRoulette = Config::russianRoulette;
        }

        Spectrum BDPathTracer::connectPath(std::shared_ptr<Scene> scene,
                                           PathVertex *cameraSubPath, int cameraPathLength, int t,
                                           PathVertex *lightSubPath, int lightPathLength, int s,
                                           Point2F *samplePosition, Sampler *sampler) {
            // 检查 t 和 s 的范围，必须处于 cameraPath 和 lightPathLength 的长度范围之中
            assert(t >= 1 && t <= cameraPathLength);
            assert(s >= 0 && s <= lightPathLength);

            if (t > 1 && s != 0 && cameraSubPath[t - 1].type == PathVertexType::LIGHT) {
                return Spectrum(0.);
            }

            /* 处理策略
             * Case s = 0 && t = 0:
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

            // New sample
            PathVertex extraVertex;

            Spectrum ret = Spectrum(0.0);
            if (s == 0) {
                // lightSubPath 上没有任何路径点
                const PathVertex &pt = cameraSubPath[t - 1];
                if (pt.isLight()) {
                    ret = pt.Le(cameraSubPath[t - 2].point) * pt.beta;
                }
            } else if (t == 1) {
                // 去 lightSubPath 连接点
                const PathVertex &ps = lightSubPath[s - 1];
                if (ps.isConnectible()) {
                    Vector3F worldWi;
                    Float pdf;
                    VisibilityTester visibilityTester;
                    // 对相机采样
                    Spectrum we = _camera->sampleCameraRay(ps.getInteraction(), &worldWi, &pdf, samplePosition,
                                                           sampler, &visibilityTester);
                    if (pdf > 0 && !we.isBlack() && visibilityTester.isVisible(scene)) {
                        Ray newRay = visibilityTester.getEnd().sendRay(-worldWi);
                        extraVertex = PathVertex::createCameraVertex(_camera.get(), newRay, we / pdf);
                        ret = ps.beta * ps.f(extraVertex) * extraVertex.beta;
                        if (ps.type == PathVertexType::SURFACE) {
                            ret *= ABS_DOT(ps.shadingNormal(), worldWi);
                        }

                        // transmittance
                        if (!ret.isBlack()) {
                            ret *= visibilityTester.transmittance(scene, sampler);
                        }
                    }
                }
            } else if (s == 1) {
                // 由于 lightSubPath 没有任何点参与路径构建，所以我们重新采样一个光源点，加到 cameraSubPath 上
                PathVertex &pt = cameraSubPath[t - 1];
                if (pt.isConnectible()) {
                    Float lightPdf = 0;
                    auto light = uniformSampleLight(scene, &lightPdf, sampler);
                    if (light == nullptr || lightPdf == 0.) {
                        return 0;
                    }

                    Vector3F worldWi;
                    // 光源采样处的 pdf（这里指 pt 点的入射角 wi 的 pdf)
                    Float sampleLightPdf;
                    VisibilityTester visibilityTester;
                    // 直接对光源采样，同时 visibilityTester 会保存两端交点
                    Spectrum sampleIntensity = light->sampleLi(pt.getInteraction(), &worldWi, &sampleLightPdf,
                                                               sampler, &visibilityTester);

                    if (sampleLightPdf == 0. || sampleIntensity.isBlack() ||
                        !visibilityTester.isVisible(scene)) {
                        // 没有任何光源亮度贡献的情况，直接返回 0
                        ret = Spectrum(0.0);
                    } else {
                        // 构建一个新 StartEndInteraction，用于保存路径点上的光源
                        StartEndInteraction ei = StartEndInteraction(light.get(), visibilityTester.getEnd());
                        Spectrum L = (sampleIntensity / (sampleLightPdf * lightPdf));
                        // 由于对光源采样位置是一个新点，所以要重新创建 PathVertex
                        extraVertex = PathVertex::createLightVertex(ei, L);
                        // 计算 pdfForward
                        extraVertex.pdfForward = extraVertex.densityLightOrigin(pt);

                        ret = pt.beta * pt.f(extraVertex) * extraVertex.beta;
                        if (pt.type == PathVertexType::SURFACE) {
                            ret *= std::abs(DOT(pt.shadingNormal(), worldWi));
                        }

                        // transmittance
                        if (!ret.isBlack()) {
                            ret *= visibilityTester.transmittance(scene, sampler);
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
                    ret = pt.beta * pt.f(ps) * ps.f(pt) * ps.beta * g(pt, ps, sampler);
                } else {
                    ret = Spectrum(0.0);
                }
            }

            Float misWt = ret.isBlack() ? 0.0 : misWeight(cameraSubPath, t, lightSubPath, s, extraVertex);
            ret *= misWt;
            return ret;
        }

        int BDPathTracer::generateCameraPath(std::shared_ptr<Scene> scene, const Ray &ray,
                                             std::shared_ptr<Camera> camera, PathVertex *cameraSubPath,
                                             int maxDepth, Sampler *sampler, MemoryArena &memoryArena) {
            assert(cameraSubPath != nullptr);
            // 初始 beta
            Spectrum beta = Spectrum(1.0);
            // 第一个点 Camera
            PathVertex cameraVertex = PathVertex::createCameraVertex(camera.get(), ray, beta);
            cameraSubPath[0] = cameraVertex;
            // 初始射线
            Ray scatterRay = ray;

            Float pdfPos, pdfDir;
            camera->pdfWe(scatterRay, pdfPos, pdfDir);

            return randomWalk(scene, scatterRay, cameraSubPath, maxDepth, pdfDir,
                              sampler, memoryArena, beta, TransportMode::RADIANCE);
        }

        int BDPathTracer::generateLightPath(std::shared_ptr<Scene> scene,
                                            PathVertex *lightSubPath, int maxDepth,
                                            Sampler *sampler,
                                            MemoryArena &memoryArena) {
            assert(lightSubPath != nullptr);

            Float lightPdf = 0;
            auto light = uniformSampleLight(scene, &lightPdf, sampler);
            if (light == nullptr || lightPdf == 0.) {
                return 0;
            }

            // 光源位置采样概率
            Float pdfPos = 0;
            // 光线发射方向采样概率
            Float pdfDir = 0;
            // 光源发射光线切面的法线
            Vector3F lightNormal;
            // 光源发射光线
            Ray scatterRay;
            // 采样光源发射
            Spectrum intensity = light->sampleLe(&scatterRay, &lightNormal,
                                                 &pdfPos, &pdfDir, sampler);
            if (pdfPos == 0 || pdfDir == 0 || intensity.isBlack()) {
                return 0;
            }
            // 创建光源点
            // TODO 没有考虑多光源情况
            lightSubPath[0] = PathVertex::createLightVertex(light.get(), scatterRay.getOrigin(),
                                                            scatterRay.getDirection(), lightNormal, intensity,
                                                            pdfPos * lightPdf);

            Spectrum beta = intensity * std::abs(DOT(scatterRay.getDirection(), lightNormal)) / (pdfPos * pdfDir * lightPdf);

            return randomWalk(scene, scatterRay, lightSubPath, maxDepth, pdfDir,
                              sampler, memoryArena, beta, TransportMode::IMPORTANCE);
        }

        int BDPathTracer::randomWalk(std::shared_ptr<Scene> scene, const Ray &ray,
                                     PathVertex *path, int maxDepth, Float pdf,
                                     Sampler *const sampler,
                                     MemoryArena &memoryArena,
                                     Spectrum &beta, TransportMode mode) {
            // 上个路径点发射射线的 pdf
            Float pdfPreWi = pdf;
            Float pdfWo = 0;
            Ray scatterRay = ray;
            int vertexCount = 1;
            for (int depth = 1; depth < maxDepth; depth++) {
                SurfaceInteraction si;
                bool isIntersected = scene->intersect(scatterRay, si);

                // sample1d medium interaction
                MediumInteraction mi;
                if (scatterRay.getMedium() != nullptr) {
                    beta *= scatterRay.getMedium()->sampleInteraction(scatterRay, sampler, &mi, memoryArena);
                }

                if (beta.isBlack()) {
                    break;
                }

                PathVertex &vertex = path[depth];
                PathVertex &preVertex = path[depth - 1];

                if (mi.isValid()) {
                    /* handle medium */
                    // create medium vertex
                    vertex = PathVertex::createMediumVertex(mi, pdfPreWi, preVertex, beta);
                    vertexCount++;

                    // generate next ray
                    Vector3F worldWo = -scatterRay.getDirection();
                    Vector3F worldWi;
                    pdfPreWi = mi.getPhaseFunction()->sampleScatter(worldWo, &worldWi, sampler);
                    scatterRay = mi.sendRay(worldWi);

                    // update backward pdf and density
                    pdfWo = pdfPreWi;
                } else {
                    if (isIntersected) {
                        // skip medium boundary
                        if (si.getMaterial() == nullptr) {
                            scatterRay = si.sendRay(scatterRay.getDirection());
                            depth--;
                            continue;
                        }

                        // build BSDF
                        si.buildScatteringFunction(memoryArena, mode);
                        assert(si.bsdf != nullptr);

                        // 添加新点 TODO 默认只有 Surface 类型
                        vertex = PathVertex::createSurfaceVertex(si, pdfPreWi, preVertex, beta);
                        vertexCount++;

                        // 采样下一个射线
                        Vector3F worldWo = -si.direction;
                        Vector3F worldWi = Vector3F(0);
                        BXDFType sampleType;
                        Spectrum f = si.bsdf->sampleF(worldWo, &worldWi, &pdfPreWi, sampler,
                                                      BXDFType::BSDF_ALL, &sampleType);

                        // 判断采样是否有效
                        if (pdfPreWi == 0. || f.isBlack()) {
                            break;
                        }

                        // 设置新的散射光线
                        scatterRay = si.sendRay(worldWi);

                        // TODO cosine 的计算感觉有问题，对于从光源发射的光线，应该用 worldWo
                        // 更新 beta
                        Float cosine = ABS_DOT(si.rendering.normal, worldWi);

                        beta *= (f * cosine / pdfPreWi);

                        // 计算向后 pdfWo
                        pdfWo = si.bsdf->samplePdf(worldWi, worldWo);

                        // 如果 bsdf 反射含有 delta 成分，则前后 pdf 都赋值为 0
                        if (sampleType & BXDFType::BSDF_SPECULAR) {
                            pdfWo = 0;
                            pdfPreWi = 0;
                            vertex.isDelta = true;
                        }
                    } else {
                        // TODO create background light
                        break;
                    }
                }
                // update pre-vertex's backward density
                preVertex.pdfBackward = vertex.convertToDensity(pdfWo, preVertex);
            }
            return vertexCount;
        }

        Float BDPathTracer::misWeight(PathVertex *cameraSubPath, int t,
                                       PathVertex *lightSubPath, int s,
                                       PathVertex &extraVertex) {
            if (s + t == 2) {
                // Skip t = 1 s = 1
                return 1;
            }

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
                swapper1 = {ps, extraVertex};
            } else if (t == 1) {
                swapper1 = {pt, extraVertex};
            }

            // 当 t > 0， 更新 cameraSubPath[t - 1].pdfBackward
            ScopeSwapper<Float> swapper2;
            if (pt != nullptr) {
                swapper2 = {&(pt->pdfBackward), s > 0 ? ps->computeDensity(psMinus, *pt)
                                                      : pt->densityLightOrigin(*ptMinus)};
            }

            // 当 s > 0，更新 lightSubPath[s - 1].pdfBackward
            ScopeSwapper<Float> swapper3;
            if (ps != nullptr) {
                // 这里不考虑 t 的大小，因为 t < 2 的情况是不可能进入的
                swapper3 = {&(ps->pdfBackward), pt->computeDensity(ptMinus, *ps)};
            }

            // 当 t > 2，则更新 cameraSubPath[t - 2].pdfBackward
            ScopeSwapper<Float> swapper4;
            if (ptMinus != nullptr) {
                swapper4 = {&(ptMinus->pdfBackward), s > 0 ? pt->computeDensity(ps, *ptMinus) :
                                                     pt->densityFromLight(*ptMinus)};
            }

            // 当 s > 2，则更新 lightSubPath[s - 2].pdfBackward
            ScopeSwapper<Float> swapper5;
            if (psMinus != nullptr) {
                swapper5 = {&(psMinus->pdfBackward), ps->computeDensity(pt, *psMinus)};
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
            Float sumRi = 0;
            // 临时 ri 项
            Float ri;
            // 计算 cameraSubPath 的 Ri
            ri = 1;
            for (int i = t - 1; i > 0; i--) {
                Float pdfBackward = cameraSubPath[i].pdfBackward == 0 ? 1 : cameraSubPath[i].pdfBackward;
                Float pdfForward = cameraSubPath[i].pdfForward == 0 ? 1 : cameraSubPath[i].pdfForward;
                ri *= (pdfBackward / pdfForward);

                // 当前点 cameraSubPath[i] 和 cameraSubPath[i - 1] 是连接点的时候，不应该被连起来。
                if (!cameraSubPath[i].isDelta && !cameraSubPath[i - 1].isDelta) {
                    sumRi += ri;
                }
            }

            // 计算 lightSubPath 的 Ri
            ri = 1;
            for (int i = s - 1; i >= 0; i--) {
                Float pdfBackward = lightSubPath[i].pdfBackward == 0 ? 1 : lightSubPath[i].pdfBackward;
                Float pdfForward = lightSubPath[i].pdfForward == 0 ? 1 : lightSubPath[i].pdfForward;
                ri *= (pdfBackward / pdfForward);

                // 1. 如果还没有到达最后的灯源点（i > 0），则要像 cameraSubPath 一样判断是否是 delta 分布
                // 2. 如果到达了最后的光源点 (i = 0)，则判断光源是否是 delta
                // 以上两种 delta 情况都不会进行计算 ri
                if (!lightSubPath[i].isDelta &&
                    !(i > 0 ? lightSubPath[i - 1].isDelta : lightSubPath[i].isDeltaLight())) {
                    sumRi += ri;
                }
            }

            Float ret = 1. / (sumRi + 1);
            return ret;
        }

        Spectrum BDPathTracer::g(const PathVertex &pre, const PathVertex &next, Sampler *sampler) {
            Vector3F dirToNext = next.point - pre.point;
            Float dist = LENGTH(dirToNext);
            if (dist == 0.) {
                return 1.0;
            }

            dirToNext = NORMALIZE(dirToNext);

            Float cosPre = 1.0;
            if (pre.type == PathVertexType::SURFACE) {
                cosPre = std::abs(DOT(pre.shadingNormal(), dirToNext));
            }

            Float cosNext = 1.0;
            if (next.type == PathVertexType::SURFACE) {
                cosNext = std::abs(DOT(next.shadingNormal(), dirToNext));
            }

            VisibilityTester visibilityTester = VisibilityTester(pre.getInteraction(), next.getInteraction());
            return cosPre * cosNext / std::pow(dist, 2) * visibilityTester.transmittance(_scene, sampler);
        }


        Spectrum BDPathTracer::shader(const Ray &ray, std::shared_ptr<Scene> scene, int maxDepth,
                                      Sampler *sampler, MemoryArena &memoryArena) {
            PathVertex *cameraSubPath = memoryArena.alloc<PathVertex>(maxDepth + 2, false);
            PathVertex *lightSubPath = memoryArena.alloc<PathVertex>(maxDepth, false);

            // Generate camera path
            int cameraPathLength = generateCameraPath(_scene, ray, _camera, cameraSubPath, maxDepth + 1,
                                                      sampler, memoryArena);

            // Generate light path
            int lightPathLength = generateLightPath(_scene, lightSubPath, maxDepth, sampler, memoryArena);

            Spectrum shaderColor = Spectrum(0.0);
            // Connect path
            for (int t = 1; t <= cameraPathLength; t++) {
                for (int s = 0; s <= lightPathLength; s++) {
                    int depth = t + s - 2;
                    if ((s == 1 && t == 1) || depth < 0 || depth > maxDepth) {
                        continue;
                    }

                    Point2F samplePosition;
                    Spectrum value = connectPath(scene, cameraSubPath, cameraPathLength, t,
                                                 lightSubPath, lightPathLength, s,
                                                 &samplePosition, sampler);

                    if (t == 1) {
                        // 在成像平面的 samplePosition 位置加上 value
                        _filmPlane->addExtra(value, std::floor(samplePosition.y), std::floor(samplePosition.x));
                    } else {
                        shaderColor += value;
                    }
                }
            }
            return shaderColor;
        }

    }
}