//
// Created by Storm Phoenix on 2020/10/22.
//

#include <kaguya/core/light/DiffuseAreaLight.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace kaguya {
    namespace core {

        std::shared_ptr<AreaLight> DiffuseAreaLight::buildDiffuseAreaLight(const Spectrum &intensity,
                                                                           std::shared_ptr<Geometry> geometry,
                                                                           const MediumBound &mediumBoundary,
                                                                           bool singleSide) {
            std::shared_ptr<AreaLight> light =
                    std::make_shared<DiffuseAreaLight>(intensity, geometry, mediumBoundary, singleSide);
            geometry->setAreaLight(light);
            return light;
        }

        DiffuseAreaLight::DiffuseAreaLight(const Spectrum &intensity,
                                           std::shared_ptr<Shape> shape,
                                           const MediumBound &mediumBoundary,
                                           bool singleSide) :
                AreaLight(intensity, shape, AREA, mediumBoundary), _singleSide(singleSide) {}

        Spectrum DiffuseAreaLight::randomLightRay(Ray *ray, Vector3 *normal, double *pdfPos, double *pdfDir,
                                                  const Sampler1D *sampler1D) {
            assert(_shape != nullptr);

            // 采样位置
            SurfaceInteraction si = _shape->sampleSurfacePoint(sampler1D);
            *normal = si.getNormal();

            // 采样位置 pdf
            *pdfPos = _shape->surfacePointPdf(si);

            Vector3 dirLocal;
            // 判断区域光是否是双面发光
            if (_singleSide) {
                // 单面发光，按照 cosine 方式采样
                dirLocal = hemiCosineSampling(sampler1D);
                (*pdfDir) = hemiCosineSamplePdf(dirLocal);
            } else {
                // 双面发光，按照 cosine 方式在两边采样
                dirLocal = hemiCosineSampling(sampler1D);
                (*pdfDir) = hemiCosineSamplePdf(dirLocal) * 0.5;

                // 按照 0.5 的概率确认在上/下球面做 cosine / PI 采样
                double prob = sampler1D->sample();
                if (prob < 0.5) {
                    dirLocal.y *= -1;
                }
            }

            // 计算切线坐标系
            Vector3 tanY = *normal;
            Vector3 tanX;
            Vector3 tanZ;
            tangentSpace(tanY, &tanX, &tanZ);

            // 射线方向从局部坐标系转化到世界坐标系
            Vector3 dirWorld = dirLocal.x * tanX + dirLocal.y * tanY + dirLocal.z * tanZ;

            // 设置 ray
            (*ray) = Ray(si.getPoint(), NORMALIZE(dirWorld),
                         DOT(dirWorld, *normal) > 0 ? _mediumBoundary.outside() : _mediumBoundary.inside());

            return lightRadiance(si, dirWorld);
        }

        void DiffuseAreaLight::randomLightRayPdf(const Ray &ray, const Vector3 &normal,
                                                 double *pdfPos, double *pdfDir) const {
            assert(_shape != nullptr);
            // 创建 SurfaceInteraction
            SurfaceInteraction si;
            si.setPoint(ray.getOrigin());
            (*pdfPos) = _shape->surfacePointPdf(si);
            double cosTheta = ABS_DOT(normal, ray.getDirection());
            (*pdfDir) = _singleSide ? hemiCosineSamplePdf(cosTheta) : 0.5 * hemiCosineSamplePdf(cosTheta);
        }

        Spectrum DiffuseAreaLight::lightRadiance(const Interaction &interaction, const Vector3 &wo) const {
            double cosine = DOT(interaction.getNormal(), wo);
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