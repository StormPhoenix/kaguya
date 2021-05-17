//
// Created by Storm Phoenix on 2020/10/22.
//

#include <kaguya/core/light/DiffuseAreaLight.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace kaguya {
    namespace core {

        DiffuseAreaLight::DiffuseAreaLight(const Spectrum &intensity,
                                           std::shared_ptr<Shape> shape,
                                           const MediumInterface &mediumBoundary,
                                           bool singleSide) :
                AreaLight(intensity, shape, AREA, mediumBoundary), _singleSide(singleSide) {}

        Spectrum DiffuseAreaLight::sampleLe(Ray *ray, Vector3F *normal, Float *pdfPos, Float *pdfDir,
                                            Sampler *sampler) {
            assert(_shape != nullptr);

            // 采样位置
            SurfaceInteraction si = _shape->sampleSurfacePoint(pdfPos, sampler);
            si.setMediumBoundary(_mediumInterface);

            if (normal != nullptr) {
                *normal = si.normal;
            }

            Vector3F dirLocal;
            // 判断区域光是否是双面发光
            if (_singleSide) {
                // 单面发光，按照 cosine 方式采样
                dirLocal = math::sampling::hemiCosineSampling(sampler);
                (*pdfDir) = math::sampling::hemiCosineSamplePdf(dirLocal);
            } else {
                // 双面发光，按照 cosine 方式在两边采样
                dirLocal = math::sampling::hemiCosineSampling(sampler);
                (*pdfDir) = math::sampling::hemiCosineSamplePdf(dirLocal) * 0.5;

                // 按照 0.5 的概率确认在上/下球面做 cosine / PI 采样
                Float prob = sampler->sample1D();
                if (prob < 0.5) {
                    dirLocal.y *= -1;
                }
            }

            // 计算切线坐标系
            Vector3F tanY = *normal;
            Vector3F tanX;
            Vector3F tanZ;
            math::tangentSpace(tanY, &tanX, &tanZ);

            // 射线方向从局部坐标系转化到世界坐标系
            Vector3F dirWorld = dirLocal.x * tanX + dirLocal.y * tanY + dirLocal.z * tanZ;

            // 设置 ray
            (*ray) = si.sendRay(NORMALIZE(dirWorld));
            return L(si, dirWorld);
        }

        void DiffuseAreaLight::pdfLe(const Ray &ray, const Vector3F &normal,
                                     Float *pdfPos, Float *pdfDir) const {
            assert(_shape != nullptr);
            // 创建 SurfaceInteraction
            SurfaceInteraction si;
            si.point = ray.getOrigin();
            (*pdfPos) = _shape->surfacePointPdf(si);
            Float cosTheta = ABS_DOT(normal, ray.getDirection());
            (*pdfDir) = _singleSide ? math::sampling::hemiCosineSamplePdf(cosTheta) : 0.5 *
                                                                                      math::sampling::hemiCosineSamplePdf(
                                                                                              cosTheta);
        }

        Spectrum DiffuseAreaLight::L(const Interaction &interaction, const Vector3F &wo) const {
            Float cosine = DOT(interaction.normal, wo);
            return (!_singleSide || cosine > 0) ? _intensity : Spectrum(0.0);

            /*
            if (!singleSide) {
                return _intensity;
            }

            if (cosine > 0) {
                // 同一侧
                return _intensity;
            } else {
                // 非同一侧
                return Spectrum(0.0);
            }
             */
        }

    }
}