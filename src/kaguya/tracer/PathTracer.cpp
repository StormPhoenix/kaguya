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
        }


        Spectrum PathTracer::shader2(const kaguya::tracer::Ray &ray, kaguya::Scene &scene, int depth) {
            // intersect

            // 求交点

            // 判断上次 step 是否有采样 light
            // 若没有则采样交点 / 背景

            // 判断反射终止条件 没有击中 / 超过反射深度 / 或者击中的是光源

            // 计算 BSDF

            // 判断是否是 specular
            // 不是则采样 light

            // 计算 beta / 新射线
            return Spectrum(0);
        }

        Spectrum PathTracer::shader(const Ray &ray, Scene &scene, int depth) {
            // TODO 判断采用固定深度还是轮盘赌
            // TODO 添加对光源采样；对光源采样需要计算两个 pdf

            if (depth < _maxDepth) {
                Interaction hitRecord;
                if (scene.hit(ray, hitRecord)) {
                    // 击中，检查击中材质
                    // TODO
                    std::shared_ptr<Material> material = hitRecord.material;
                    if (material->isLight()) {
                        // 发光物体
                        return material->emitted(hitRecord.u, hitRecord.v);
                    } else {
                        // 不发光物体
                        // 若对光源采样，则记录采样射线
                        Ray scatterRay;
                        // 若对光源采样，则记录采样概率
                        double samplePdf;

                        if (!material->isSpecular() &&
                            sampleFromLights(scene, hitRecord.point, scatterRay, samplePdf)) {
                            std::shared_ptr<BSDF> bsdf = material->bsdf(hitRecord);

                            if (bsdf == nullptr) {
                                return Spectrum(0.0);
                            }

                            // 对非光源采样进行加权
                            double scatterPdf = material->scatterPDF(ray, hitRecord, scatterRay);
                            samplePdf = _sampleLightProb * samplePdf + (1 - _sampleLightProb) * scatterPdf;


                            Spectrum f = bsdf->f(NORMALIZE(-ray.getDirection()), NORMALIZE(scatterRay.getDirection()));
                            Spectrum shaderColor =
                                    std::abs(DOT(hitRecord.normal, scatterRay.getDirection())) * f /
                                    samplePdf * shader(scatterRay, scene, depth + 1);
                            return material->emitted(hitRecord.u, hitRecord.v) + shaderColor;
                        } else {
                            if (hitRecord.id == 1) {
                                int a = 0;
                                a++;
                            }

                            std::shared_ptr<BSDF> bsdf = material->bsdf(hitRecord);
                            Vector3 worldWo = -ray.getDirection();
                            Vector3 worldWi = Vector3(0.0);
                            double samplePdf = 0;
                            Spectrum f = bsdf->sampleF(worldWo, &worldWi, &samplePdf);

                            if (samplePdf == 0 || f.isBlack()) {
                                return material->emitted(hitRecord.u, hitRecord.v);
                            }

                            // TODO 区分反射种类
                            if (!material->isSpecular()) {
                                // TODO 目前只考虑单光源的情况
                                std::shared_ptr<Light> light = scene.getLight();
                                // TODO delete
                                double tempPdf = light->rayPdf(scatterRay);
                                samplePdf = (1 - _sampleLightProb) * samplePdf +
                                            _sampleLightProb * tempPdf;
                            }

                            double cosine = std::abs(DOT(hitRecord.normal, NORMALIZE(worldWi)));

                            scatterRay.setOrigin(hitRecord.point);
                            scatterRay.setDirection(worldWi);
                            Spectrum shaderColor = f * cosine / samplePdf * shader(scatterRay, scene, depth + 1);
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

        Spectrum PathTracer::computeShaderColor(double scatterPdf, double samplePdf, Spectrum brdf, Ray &scatterRay,
                                                kaguya::Scene &scene, int depth) {
            return brdf * scatterPdf * shader(scatterRay, scene, depth) / samplePdf;
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
                light->sampleRay(shadowRayOrigin, shadowRay);
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

                            auto u = (col + uniformSample()) / cameraWidth;
                            auto v = (row + uniformSample()) / cameraHeight;

                            // 发射采样光线，开始渲染
                            Ray sampleRay = _camera->sendRay(u, v);
                            ans += shader(sampleRay, *_scene, 0);
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