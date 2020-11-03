//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/Config.h>
#include <kaguya/core/light/AreaLight.h>
#include <kaguya/material/Material.h>
#include <kaguya/scene/Shape.h>
#include <kaguya/tracer/pt/PathTracer.h>

#include <iostream>
#include <omp.h>

namespace kaguya {
    namespace tracer {

        using kaguya::core::Interaction;
        using kaguya::material::Material;

        PathTracer::PathTracer() {
            init();
        }

        void PathTracer::init() {
            _scene = Config::testBuildScene();
            _camera = _scene->getCamera();
            _samplePerPixel = Config::samplePerPixel;
            _sampleLightProb = Config::sampleLightProb;
            _maxDepth = Config::maxScatterDepth;
            _russianRouletteBounce = Config::beginRussianRouletteBounce;
            _russianRoulette = Config::russianRoulette;
        }


        Spectrum PathTracer::shaderOfProgression(const kaguya::tracer::Ray &ray, kaguya::Scene &scene,
                                                 MemoryArena &memoryArena) {
            // 最终渲染结果
            Spectrum shaderColor = Spectrum(0);
            // 光线是否是 delta distribution
            bool isSpecular = false;
            // 保存历次反射计算的 (f(p, wi, wo) * G(wo, wi)) / p(wi)
            Spectrum beta = 1.0;
            // 散射光线
            Ray scatterRay = ray;

            // TODO 依然只考虑一个光源的情况
            const std::shared_ptr<Light> light = scene.getLight();
            assert(light != nullptr);

            // 最多进行 _maxDepth 次数弹射
            for (int bounce = 0; bounce < _maxDepth; bounce++) {
                // intersect
                SurfaceInteraction intersection;
                bool isIntersected = scene.hit(scatterRay, intersection);

                // 此处参考 pbrt 的写法，需要判断 bounce = 0 和 isSpecular 两种特殊情况
                if (bounce == 0 || isSpecular) {
                    if (isIntersected) {
                        assert(intersection.material != nullptr);
                        // 如果有交点，则直接从交点上取值
                        if (intersection.areaLight != nullptr) {
                            shaderColor += (intersection.areaLight->lightRadiance(intersection, -intersection.direction) *
                                            beta);
                        }
                    } else {
                        shaderColor += (beta * background(scatterRay));
                        break;
                    }
                }

                // 终止条件判断
                if (!isIntersected) {
                    // TODO 添加环境光
                    break;
                }

                Material *material = intersection.material;
                assert(material != nullptr);

                BSDF *bsdf = intersection.buildBSDF(memoryArena);
                assert(bsdf != nullptr);

                // 判断是否向光源采样
                if (bsdf->belongToType(BXDFType(BSDF_ALL & (~BSDF_SPECULAR)))) {
                    shaderColor += (beta * evaluateDirectLight(scene, intersection, (*bsdf)));
                }

                // 计算下一次反射
                Vector3 worldWo = -scatterRay.getDirection();
                Vector3 worldWi = Vector3(0.0);
                // 材质反射类型
                BXDFType bxdfType;
                // p(wi)
                double samplePdf = 0;
                // f(p, wo, wi)
                Spectrum f = bsdf->sampleF(worldWo, &worldWi, &samplePdf, BSDF_ALL, &bxdfType);

                // cosine
                double cosine = std::abs(DOT(intersection.normal, NORMALIZE(worldWi)));
                // 计算 beta
                beta *= (f * cosine / samplePdf);
                // 设置下一次打击光线
                scatterRay.setOrigin(intersection.point);
                scatterRay.setDirection(NORMALIZE(worldWi));

                isSpecular = (bxdfType & BSDF_SPECULAR) > 0;

                if (bounce > _russianRouletteBounce && uniformSample() < _russianRoulette) {
                    beta /= (1 - _russianRoulette);
                }
            }
            return shaderColor;
        }

        Spectrum PathTracer::shaderOfRecursion(const Ray &ray, Scene &scene, int depth, MemoryArena &memoryArena) {
            // TODO 判断采用固定深度还是轮盘赌
            // TODO 添加对光源采样；对光源采样需要计算两个 surfacePointPdf

            if (depth < _maxDepth) {
                SurfaceInteraction hitRecord;
                if (scene.hit(ray, hitRecord)) {
                    // 击中，检查击中材质
                    Material *material = hitRecord.material;
                    // 不发光物体
                    // 若对光源采样，则记录采样射线
                    Ray scatterRay;
                    // 若对光源采样，则记录采样概率
                    double samplePdf = 0;

                    if (!material->isSpecular()) {
                        // 不是 Specular 类型，考虑对光源采样
                        BSDF *bsdf = hitRecord.buildBSDF(memoryArena);
                        if (bsdf == nullptr) {
                            return Spectrum(0.0);
                        }

                        bool gamblingResult = uniformSample() < _sampleLightProb;
                        if (gamblingResult) {
                            Vector3 scatterRayDir;
                            double samplePdf = 0;
                            auto light = scene.getLight();

                            VisibilityTester visibilityTester;
                            Spectrum spectrum = light->sampleFromLight(hitRecord, &scatterRayDir, &samplePdf,
                                                                       &visibilityTester);

                            if (samplePdf > EPSILON && !spectrum.isBlack()) {
                                // 计算该方向的散射 PDF
                                double scatterPdf = bsdf->samplePdf(-ray.getDirection(), scatterRayDir);
                                // 对非光源采样进行加权
                                samplePdf = _sampleLightProb * samplePdf + (1 - _sampleLightProb) * scatterPdf;
                                Spectrum f = bsdf->f(NORMALIZE(-ray.getDirection()), NORMALIZE(scatterRayDir));
                                scatterRay.setOrigin(hitRecord.point);
                                scatterRay.setDirection(NORMALIZE(scatterRayDir));
                                Spectrum shaderColor =
                                        std::abs(DOT(hitRecord.normal, NORMALIZE(scatterRayDir))) * f /
                                        samplePdf *
                                        ((depth > _russianRoulette && uniformSample() < _russianRoulette) ?
                                         Spectrum(0.0) :
                                         shaderOfRecursion(scatterRay, scene, depth + 1, memoryArena) /
                                         (1 - _russianRoulette));
                                return shaderColor + (hitRecord.areaLight != nullptr ?
                                                      hitRecord.areaLight->lightRadiance(hitRecord,
                                                                                         -hitRecord.direction) :
                                                      Spectrum(0.0));
                            }
                        }
                    }

                    BSDF *bsdf = hitRecord.buildBSDF(memoryArena);
                    Vector3 worldWo = -ray.getDirection();
                    Vector3 worldWi = Vector3(0.0);
                    Spectrum f = bsdf->sampleF(worldWo, &worldWi, &samplePdf);
                    scatterRay.setOrigin(hitRecord.point);
                    scatterRay.setDirection(NORMALIZE(worldWi));

                    if (!material->isSpecular()) {
                        // TODO 目前只考虑单光源的情况
                        std::shared_ptr<Light> light = scene.getLight();
                        Interaction eye;
                        eye.point = hitRecord.point;
                        samplePdf = (1 - _sampleLightProb) * samplePdf +
                                    _sampleLightProb * light->sampleFromLightPdf(eye, scatterRay.getDirection());
                    }

                    double cosine = std::abs(DOT(hitRecord.normal, NORMALIZE(worldWi)));
                    Spectrum shaderColor = cosine * f / samplePdf *
                                           ((depth > _russianRoulette && uniformSample() < _russianRoulette) ?
                                            Spectrum(0.0) :
                                            shaderOfRecursion(scatterRay, scene, depth + 1, memoryArena) /
                                            (1 - _russianRoulette));

                    return shaderColor + (hitRecord.areaLight != nullptr ?
                                          hitRecord.areaLight->lightRadiance(hitRecord, -hitRecord.direction) :
                                          Spectrum(0.0));
                } else {
                    // 未击中
                    return background(ray);
                }
            } else {
                // 停止散射
                return Spectrum(0.0f);
            }
        }


        Spectrum PathTracer::evaluateDirectLight(Scene &scene, const Interaction &eye, const BSDF &bsdf) {
            // TODO 目前只考虑单个光源
            auto light = scene.getLight();
            // p(wi)
            double lightPdf = 0;
            // light dir
            Vector3 shadowRayDir = Vector3(0.0);
            // visibility tester
            VisibilityTester visibilityTester;
            // 对光源采样采样
            Spectrum luminance = light->sampleFromLight(eye, &shadowRayDir,
                                                        &lightPdf,
                                                        &visibilityTester);

            // 排除对光源采样贡献为 0 的情况
            if (lightPdf > EPSILON && !luminance.isBlack()) {
                // 判断 shadowRay 是否击中
                if (visibilityTester.isVisible(scene)) {
                    // cosine
                    double cosine = std::abs(DOT(eye.normal, shadowRayDir));
                    // f(p, wo, wi)
                    Spectrum f = bsdf.f(-eye.direction, shadowRayDir);
                    if (light->isDeltaType()) {
                        // shader spectrum
                        return f * cosine / lightPdf * luminance;
                    } else {
                        // multiple importance sampling
                        double scatterPdf = bsdf.samplePdf(-eye.direction, shadowRayDir);
                        double weight = misWeight(1, lightPdf, 1, scatterPdf);
                        return f * cosine * weight / lightPdf * luminance;
                    }
                }
            }
            // 没有采样到光源
            return Spectrum(0.0);
        }

        void PathTracer::run() {
            if (_camera != nullptr && _scene != nullptr) {
                int cameraWidth = _camera->getResolutionWidth();
                int cameraHeight = _camera->getResolutionHeight();

                // TODO 将 bitmap 封装到写入策略模式
                // TODO 代码移动到 tracer
                _bitmap = (int *) malloc(cameraHeight * cameraWidth * SPECTRUM_CHANNEL * sizeof(unsigned int));
                double sampleWeight = 1.0 / _samplePerPixel;
                // 已完成扫描的行数
                int finishedLine = 0;
#pragma omp parallel for num_threads(12)
                // 遍历相机成像图案上每个像素
                for (int row = cameraHeight - 1; row >= 0; row--) {
                    MemoryArena arena;
                    for (int col = 0; col < cameraWidth; col++) {
                        Spectrum ans = {0};
                        // 做 _samplePerPixel 次采样
                        for (int sampleCount = 0; sampleCount < _samplePerPixel; sampleCount++) {
                            auto u = (col + uniformSample()) / cameraWidth;
                            auto v = (row + uniformSample()) / cameraHeight;

                            // 发射采样光线，开始渲染
                            Ray sampleRay = _camera->sendRay(u, v);
//                            ans += shaderOfRecursion(sampleFromLight, *_scene, 0, arena);
                            ans += shaderOfProgression(sampleRay, *_scene, arena);
                            arena.clean();
                        }
                        ans *= sampleWeight;
                        // TODO 写入渲染结果，用更好的方式写入
                        writeShaderColor(ans, row, col);
                    }
#pragma omp critical
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

        Spectrum PathTracer::background(const Ray &ray) {
            // TODO 临时设计背景色
            return Spectrum(0.0f);

            /*
            if (ray.getDirection().z < 0) {
                return Vector3(0.0f, 0.0f, 0.0f);
            } else {
                return Vector3(1.0f, 1.0f, 1.0f);
            }

            Vector3 dir = NORMALIZE(ray.getDirection());
            double t = 0.5 * (dir.y + 1.0);
            return (1.0 - t) * Vector3(1.0, 1.0, 1.0) + t * Vector3(0.5, 0.7, 1.0);
             */
        }

    }
}