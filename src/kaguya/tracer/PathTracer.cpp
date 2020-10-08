//
// Created by Storm Phoenix on 2020/10/1.
//

#include <kaguya/Config.h>
#include <kaguya/tracer/PathTracer.h>
#include <kaguya/scene/Hittable.h>
#include <kaguya/material/Material.h>
#include <iostream>

namespace kaguya {
    namespace tracer {

        using kaguya::scene::HitRecord;
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
                        Ray scatteredRay;
                        float samplePdf;
                        // 开始散射
                        if (material->scatter(ray, hitRecord, scatteredRay, samplePdf)) {
                            // 散射光线的 pdf
                            float scatterPdf = material->scatterPDF(ray, hitRecord, scatteredRay);
                            // 计算余弦
//                            float cosine = abs(DOT(NORMALIZE(scatteredRay.getDirection()), NORMALIZE(hitRecord.normal)));
                            // 获取材质 brdf，计算渲染结果
                            Vector3 shaderColor = material->brdf(hitRecord, scatteredRay.getDirection()) *
                                                  scatterPdf * shader(scatteredRay, scene, depth + 1) /* * cosine*/ /
                                                  samplePdf;
                            return material->emitted(hitRecord.u, hitRecord.v) + shaderColor;
                        } else {
                            // 不发生散射
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
                        // 写入渲染结果
                        writeShaderColor(ans);
                    }
                }
            }
        }

        Vector3 PathTracer::background(const Ray &ray) {
            // TODO 临时设计背景色
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