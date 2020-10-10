//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/Config.h>
#include <kaguya/tracer/PathTracer.h>
#include <kaguya/scene/Hittable.h>
#include <kaguya/scene/meta/Light.h>
#include <kaguya/material/Material.h>
#include <iostream>

namespace kaguya {
    namespace tracer {

        using kaguya::scene::HitRecord;
        using kaguya::scene::Light;
        using kaguya::material::Material;

        PathTracer::PathTracer() {
            init();
        }

        void PathTracer::init() {
            _scene = Config::testBuildTwoScene();
            _camera = _scene->getCamera();
        }

        Vector3 PathTracer::shader(const Ray &ray, Scene &scene, int depth) {
            // TODO 判断采用固定深度还是轮盘赌
            // TODO 添加对光源采样；对光源采样需要计算两个 pdf

            if (depth < _maxDepth) {
                HitRecord hitRecord;
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
                        double samplePdf;

                        if (!material->isSpecular() &&
                            sampleFromLights(scene, hitRecord.point, scatterRay, samplePdf)) {
                            // 对光源采样
                            float scatterPdf = material->scatterPDF(ray, hitRecord, scatterRay);
                            samplePdf = _sampleLightProb * samplePdf + (1 - _sampleLightProb) * scatterPdf;

                            Vector3 brdf = material->brdf(hitRecord, scatterRay.getDirection());

                            Vector3 shaderColor = computeShaderColor(scatterPdf, samplePdf, brdf, scatterRay, scene,
                                                                     depth + 1);
                            return material->emitted(hitRecord.u, hitRecord.v) + shaderColor;
                        } else if (material->scatter(ray, hitRecord, scatterRay, samplePdf)) {
                            // 不对光源采样，自由散射
                            // 散射光线的 pdf
                            float scatterPdf = material->scatterPDF(ray, hitRecord, scatterRay);

                            // TODO 区分反射种类
                            if (!material->isSpecular()) {
                                // TODO 目前只考虑单光源的情况
                                std::shared_ptr<Light> light = scene.getLight();
                                // TODO delete
                                double tempPdf = light->rayPdf(scatterRay);
                                samplePdf = (1 - _sampleLightProb) * samplePdf +
                                            _sampleLightProb * tempPdf;
                            }

                            Vector3 brdf = material->brdf(hitRecord, scatterRay.getDirection());
                            // 获取材质 brdf，计算渲染结果
                            Vector3 shaderColor = computeShaderColor(scatterPdf, samplePdf, brdf, scatterRay, scene,
                                                                     depth + 1);
                            return material->emitted(hitRecord.u, hitRecord.v) + shaderColor;
                        } else {
                            // 由于材质的一些原因，不会发生散射
                            return material->emitted(hitRecord.u, hitRecord.v);
                        }
                    }
                } else {
                    // 未击中
                    return background(ray);
                }
            } else {
                // 停止散射
                return Vector3(0.0f, 0.0f, 0.0f);
            }
        }

        Vector3 PathTracer::computeShaderColor(double scatterPdf, double samplePdf, Vector3 brdf, Ray &scatterRay,
                                               kaguya::Scene &scene, int depth) {
            return brdf * float(scatterPdf) * shader(scatterRay, scene, depth) / float(samplePdf);
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
                light->sampleRay(shadowRayOrigin, shadowRay, shadowRayPdf);

                sampleLightRay = shadowRay;
                sampleLightPdf = shadowRayPdf;
                return true;
            } else {
                // gambling 失败，不对光源采样
                return false;
            }
//             */

            /*
                bool gamblingResult = uniformSample() < _sampleLightProb;
                if (gamblingResult) {
                    // TODO 暂时只考虑一个光源的情况
                    const std::shared_ptr<Light> light = scene.getLight();
                    // shadow ray 起点
                    Vector3 shadowRayOrigin = sampleObject;
                    // shadow ray 投射的方向
                    Vector3 shadowRayDir;
                    // shadow ray 投射的密度
                    double shadowRayPdf;
                    if (light->sampleRay(shadowRayOrigin, shadowRayDir, shadowRayPdf)) {
                        // 构造虚拟 shadow ray
                        Ray shadowRay = Ray(shadowRayOrigin, shadowRayDir);
                        // shadow ray 击中记录
                        HitRecord shadowRayHitRecord;
                        if (scene.hit(shadowRay, shadowRayHitRecord)) {
                            if (shadowRayHitRecord.id == light->getId()) {
                                // 成功击中光源
                                sampleLightRay = shadowRay;
                                sampleLightPdf = shadowRayPdf;
                                return true;
                            } else {
                                // 光源被挡住，击中的是其他物体
                                return false;
                            }
                        } else {
                            // 没有击中任何物体
                            return false;
                        }
                    } else {
                        // 无法对光源采样
                        return false;
                    }
                } else {
                    return false;
                }
                */
        }


        void PathTracer::run() {
            if (_camera != nullptr && _scene != nullptr) {
                int cameraWidth = _camera->getResolutionWidth();
                int cameraHeight = _camera->getResolutionHeight();

                // TODO delete
                std::cout << "P3\n" << cameraWidth << " " << cameraHeight << "\n255\n";

                // 遍历相机成像图案上每个像素
                for (int row = cameraHeight - 1; row >= 0; row--) {

                    // TODO delete
                    std::cerr << "\rScanlines remaining: " << row << "  " << std::flush;


                    for (int col = 0; col < cameraWidth; col++) {
                        Vector3 ans = {0, 0, 0};
                        // 做 _samplePerPixel 次采样
                        for (int sampleCount = 0; sampleCount < _samplePerPixel; sampleCount++) {

                            auto u = (col + uniformSample()) / cameraWidth;
                            auto v = (row + uniformSample()) / cameraHeight;

//                            float u = float(col) / cameraWidth;
//                            float v = float(row) / cameraHeight;

                            // 发射采样光线，开始渲染
                            Ray sampleRay = _camera->sendRay(u, v);
                            ans += shader(sampleRay, *_scene, 0);
                        }
                        ans /= _samplePerPixel;
                        // TODO 写入渲染结果，用更好的方式写入
                        writeShaderColor(ans);
                    }
                }
            }
        }

        Vector3 PathTracer::background(const Ray &ray) {
            // TODO 临时设计背景色
            return Vector3(0.0f, 0.0f, 0.0f);

            Vector3 dir = NORMALIZE(ray.getDirection());
            float t = 0.5 * (dir.y + 1.0);
            return float(1.0 - t) * Vector3(1.0, 1.0, 1.0) + t * Vector3(0.5, 0.7, 1.0);
        }

        void PathTracer::writeShaderColor(const Vector3 &color) {
            auto r = sqrt(color.x);
            auto g = sqrt(color.y);
            auto b = sqrt(color.z);

            // Write the translated [0,255] value of each color component.
            std::cout << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
                      << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
                      << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
        }

    }
}