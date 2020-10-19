//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/Config.h>
#include <kaguya/tracer/PathTracer.h>
#include <kaguya/scene/Shape.h>
#include <kaguya/scene/meta/Light.h>
#include <kaguya/material/Material.h>
#include <iostream>

#include <omp.h>

namespace kaguya {
    namespace tracer {

        using kaguya::core::Interaction;
        using kaguya::scene::Light;
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


        Spectrum PathTracer::shaderOfProgression(const kaguya::tracer::Ray &ray, kaguya::Scene &scene) {
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
            // TODO 修改 Russian Roulette，不设置终止条件
            for (int bounce = 0; bounce < _maxDepth; bounce++) {
                // intersect
                Interaction intersection;
                bool isIntersected = scene.hit(scatterRay, intersection);

                // 此处参考 pbrt 的写法，需要判断 bounce = 0 和 isSpecular 两种特殊情况
                if (bounce == 0 || isSpecular) {
                    if (isIntersected) {
                        assert(intersection.material != nullptr);
                        // 如果有交点，则直接从交点上取值
                        shaderColor += (beta * intersection.material->emitted(intersection.u, intersection.v));
                        // 判断交点是否是光源
                        if (intersection.material->isLight()) {
                            break;
                        }
                    } else {
                        shaderColor += (beta * background(scatterRay));
                        break;
                    }
                }

                // 终止条件判断
                if (!isIntersected) {
                    break;
                }

                // 判断击中的是否是光源
                if (intersection.material->isLight()) {
                    /*
                     * TODO 还没有完全认清楚 Light 在 scene 中是否应该作为反射物质来判断
                     * 这里再加上 对光源采样，相当于在同一深度对光源采样了两次
                     */
//                    shaderColor += (beta * intersection.material->emitted(intersection.u, intersection.v));
                    break;
                }

                std::shared_ptr<Material> material = intersection.material;
                assert(material != nullptr);

                std::shared_ptr<BSDF> bsdf = material->bsdf(intersection);
                assert(bsdf != nullptr);

                // 判断是否向光源采样
                if (bsdf->belongToType(BXDFType(BSDF_ALL & (~BSDF_SPECULAR)))) {
                    // TODO 依然没有考虑到 shadow ray 是否可以击中光源的情况
                    Ray shadowRay;
                    // p(wi)
                    double shadowRayPdf = 0;
                    // 对光源采样采样，不同类型光源的采样方式不一样
                    if (light->sampleRay(intersection.point, shadowRay)) {
                        // 判断 shadowRay 是否击中
                        Interaction shadowInteraction;
                        bool lightIntersected = scene.hit(shadowRay, shadowInteraction);
                        if (lightIntersected && shadowInteraction.id == light->getId()) {
                            shadowRayPdf = light->rayPdf(shadowRay);
                            // 防止 shadowRayPdf = 0
                            if (std::abs(shadowRayPdf - 0) > EPSILON) {
                                // cosine
                                double cosine = std::abs(DOT(intersection.normal, shadowRay.getDirection()));
                                // f(p, wo, wi)
                                Spectrum f = bsdf->f(-scatterRay.getDirection(), shadowRay.getDirection());
                                // shaderOfRecursion spectrum
                                shaderColor += (beta * f * cosine / shadowRayPdf *
                                                light->luminance(intersection.u, intersection.v));
                            }
                        }
                    }
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

        Spectrum PathTracer::shaderOfRecursion(const Ray &ray, Scene &scene, int depth) {
            // TODO 判断采用固定深度还是轮盘赌
            // TODO 添加对光源采样；对光源采样需要计算两个 pdf

            if (depth < _maxDepth) {
                Interaction hitRecord;
                if (scene.hit(ray, hitRecord)) {
                    // 击中，检查击中材质
                    std::shared_ptr<Material> material = hitRecord.material;
                    if (material->isLight()) {
                        // 发光物体
                        return material->emitted(hitRecord.u, hitRecord.v);
                    } else {
                        // 不发光物体
                        // 若对光源采样，则记录采样射线
                        Ray scatterRay;
                        // 若对光源采样，则记录采样概率
                        double samplePdf = 0;

                        if (!material->isSpecular() &&
                            sampleFromLights(scene, hitRecord.point, scatterRay, samplePdf)) {
                            std::shared_ptr<BSDF> bsdf = material->bsdf(hitRecord);

                            if (bsdf == nullptr) {
                                return Spectrum(0.0);
                            }

                            // 对非光源采样进行加权
                            double scatterPdf = bsdf->samplePdf(-ray.getDirection(), scatterRay.getDirection());
                            samplePdf = _sampleLightProb * samplePdf + (1 - _sampleLightProb) * scatterPdf;

                            Spectrum f = bsdf->f(NORMALIZE(-ray.getDirection()), NORMALIZE(scatterRay.getDirection()));
                            Spectrum shaderColor =
                                    std::abs(DOT(hitRecord.normal, scatterRay.getDirection())) * f /
                                    samplePdf * shaderOfRecursion(scatterRay, scene, depth + 1);

                            return material->emitted(hitRecord.u, hitRecord.v) + shaderColor;
                        } else {
                            std::shared_ptr<BSDF> bsdf = material->bsdf(hitRecord);
                            Vector3 worldWo = -ray.getDirection();
                            Vector3 worldWi = Vector3(0.0);
                            Spectrum f = bsdf->sampleF(worldWo, &worldWi, &samplePdf);
                            scatterRay.setOrigin(hitRecord.point);
                            scatterRay.setDirection(NORMALIZE(worldWi));

                            // TODO 区分反射种类
                            if (!material->isSpecular()) {
                                // TODO 目前只考虑单光源的情况
                                std::shared_ptr<Light> light = scene.getLight();
                                samplePdf = (1 - _sampleLightProb) * samplePdf +
                                            _sampleLightProb * light->rayPdf(scatterRay);
                            }

                            double cosine = std::abs(DOT(hitRecord.normal, NORMALIZE(worldWi)));
                            Spectrum shaderColor = cosine * f / samplePdf *
                                    shaderOfRecursion(scatterRay, scene, depth + 1);
                            return material->emitted(hitRecord.u, hitRecord.v) + shaderColor;
                        }
                    }
                } else {
                    // 未击中
                    return background(ray);
                }
            } else {
                // 停止散射
                return Spectrum(0.0f);
            }
        }

        bool PathTracer::sampleFromLights(Scene &scene, Vector3 sampleObject, Ray &sampleLightRay,
                                          double &sampleLightPdf) {
//            /*
            // 最简单的实现，不使用 shadow ray
            bool gamblingResult = uniformSample() < _sampleLightProb;
            if (gamblingResult) {
                // TODO 暂时只考虑一个光源的情况
                const std::shared_ptr<Light> light = scene.getLight();
                // shadow ray 起点
                Vector3 shadowRayOrigin = sampleObject;
                // shadow ray 投射的密度
                double shadowRayPdf;
                // 构造虚拟 shadow ray
                Ray shadowRay;
                if (!light->sampleRay(shadowRayOrigin, shadowRay)) {
                    // 无法采样到光线
                    return false;
                }
                shadowRayPdf = light->rayPdf(shadowRay);

                sampleLightRay = shadowRay;
                sampleLightPdf = shadowRayPdf;
                return true;
            } else {
                // gambling 失败，不对光源采样
                return false;
            }
        }


        void PathTracer::run() {
            if (_camera != nullptr && _scene != nullptr) {
                int cameraWidth = _camera->getResolutionWidth();
                int cameraHeight = _camera->getResolutionHeight();

                // TODO 将 bitmap 封装到写入策略模式
                _bitmap = (int *) malloc(cameraHeight * cameraWidth * SPECTRUM_CHANNEL * sizeof(unsigned int));

                double sampleWeight = 1.0 / _samplePerPixel;
                // 遍历相机成像图案上每个像素
                // 已完成扫描的行数
                int finishedLine = 0;
#pragma omp parallel for num_threads(4)
                for (int row = cameraHeight - 1; row >= 0; row--) {
                    for (int col = 0; col < cameraWidth; col++) {
                        Spectrum ans = {0};

                        // 做 _samplePerPixel 次采样
                        for (int sampleCount = 0; sampleCount < _samplePerPixel; sampleCount++) {
//                            auto u = (col + uniformSample()) / cameraWidth;
//                            auto v = (row + uniformSample()) / cameraHeight;

                            auto u = (col + uniformSample() * 0.5) / cameraWidth;
                            auto v = (row + uniformSample() * 0.5) / cameraHeight;

                            // 发射采样光线，开始渲染
                            Ray sampleRay = _camera->sendRay(u, v);
//                            ans += shaderOfRecursion(sampleRay, *_scene, 0);
                            ans += shaderOfProgression(sampleRay, *_scene);
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

        void PathTracer::writeShaderColor(const Spectrum &spectrum, int row, int col) {
            assert(_bitmap != nullptr && _camera != nullptr);
            int offset = (row * _camera->getResolutionWidth() + col) * SPECTRUM_CHANNEL;
            for (int channel = 0; channel < SPECTRUM_CHANNEL; channel++) {
                *(_bitmap + offset + channel) = static_cast<int>(256 * clamp(std::sqrt(spectrum[channel]), 0.0, 0.999));
            }
        }

    }
}