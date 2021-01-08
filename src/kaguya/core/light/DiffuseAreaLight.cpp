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
                                           std::shared_ptr<Geometry> shape,
                                           const MediumBound &mediumBoundary,
                                           bool singleSide) :
                AreaLight(intensity, shape, AREA, mediumBoundary), _singleSide(singleSide) {}

        Spectrum DiffuseAreaLight::randomLightRay(Ray *ray, Vector3d *normal, double *pdfPos, double *pdfDir,
                                                  Sampler *sampler1D) {
            assert(_geometry != nullptr);

            // 采样位置
            SurfaceInteraction si = _geometry->getShape()->sampleSurfacePoint(sampler1D);
            *normal = si.getNormal();

            // 采样位置 pdf
            *pdfPos = _geometry->getShape()->surfacePointPdf(si);

            Vector3d dirLocal;
            // 判断区域光是否是双面发光
            if (_singleSide) {
                // 单面发光，按照 cosine 方式采样
                dirLocal = math::hemiCosineSampling(sampler1D);
                (*pdfDir) = math::hemiCosineSamplePdf(dirLocal);
            } else {
                // 双面发光，按照 cosine 方式在两边采样
                dirLocal = math::hemiCosineSampling(sampler1D);
                (*pdfDir) = math::hemiCosineSamplePdf(dirLocal) * 0.5;

                // 按照 0.5 的概率确认在上/下球面做 cosine / PI 采样
                double prob = sampler1D->sample1D();
                if (prob < 0.5) {
                    dirLocal.y *= -1;
                }
            }

            // 计算切线坐标系
            Vector3d tanY = *normal;
            Vector3d tanX;
            Vector3d tanZ;
            math::tangentSpace(tanY, &tanX, &tanZ);

            // 射线方向从局部坐标系转化到世界坐标系
            Vector3d dirWorld = dirLocal.x * tanX + dirLocal.y * tanY + dirLocal.z * tanZ;

            // 设置 ray
            (*ray) = Ray(si.getPoint(), NORMALIZE(dirWorld),
                         DOT(dirWorld, *normal) > 0 ? _mediumBoundary.outside() : _mediumBoundary.inside());

            return lightRadiance(si, dirWorld);
        }

        void DiffuseAreaLight::randomLightRayPdf(const Ray &ray, const Vector3d &normal,
                                                 double *pdfPos, double *pdfDir) const {
            assert(_geometry != nullptr);
            // 创建 SurfaceInteraction
            SurfaceInteraction si;
            si.setPoint(ray.getOrigin());
            (*pdfPos) = _geometry->getShape()->surfacePointPdf(si);
            double cosTheta = ABS_DOT(normal, ray.getDirection());
            (*pdfDir) = _singleSide ? math::hemiCosineSamplePdf(cosTheta) : 0.5 * math::hemiCosineSamplePdf(cosTheta);
        }

        Spectrum DiffuseAreaLight::lightRadiance(const Interaction &interaction, const Vector3d &wo) const {
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