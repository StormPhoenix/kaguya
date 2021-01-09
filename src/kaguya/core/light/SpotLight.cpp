//
// Created by Storm Phoenix on 2020/10/25.
//

#include <kaguya/core/light/SpotLight.h>

namespace kaguya {
    namespace core {

        SpotLight::SpotLight(const Vector3d center, const Vector3d dir, Spectrum intensity,
                             const MediumBound &mediumBoundary, double fallOffRange, double totalRange) :
                Light(DELTA_POSITION, mediumBoundary),
                _center(center), _dir(NORMALIZE(dir)),
                _intensity(intensity) {
            assert(_mediumBoundary.inside() == _mediumBoundary.outside());

            _cosFallOffRange = std::cos(math::DEGREES_TO_RADIANS(fallOffRange));
            _cosTotalRange = std::cos(math::DEGREES_TO_RADIANS(totalRange));
        }

        Spectrum SpotLight::sampleFromLight(const Interaction &eye,
                                            Vector3d *wi, double *pdf,
                                            Sampler *sampler1D,
                                            VisibilityTester *visibilityTester) {
            (*wi) = NORMALIZE(_center - eye.getPoint());
            (*pdf) = 1.0;

            Vector3d samplePoint = _center;
            Vector3d sampleDir = NORMALIZE(_center - eye.getPoint());
            Vector3d sampleNormal = -sampleDir;
            Interaction interaction = Interaction(samplePoint, sampleDir, sampleNormal, _mediumBoundary);

            (*visibilityTester) = VisibilityTester(eye, interaction);
            return _intensity * fallOffWeight(-(*wi)) / std::pow(LENGTH(_center - eye.getPoint()), 2);
        }

        double SpotLight::sampleFromLightPdf(const Interaction &eye, const Vector3d &dir) {
            return 0;
        }

        Spectrum SpotLight::randomLightRay(Ray *ray, Vector3d *normal, double *pdfPos, double *pdfDir,
                                           Sampler *sampler1D) {
            // 在局部坐标空间中均匀采样射线
            Vector3d dirLocal = math::sampling::coneUniformSampling(_cosTotalRange, sampler1D);

            // 计算切线空间
            Vector3d tanY = _dir;
            Vector3d tanX;
            Vector3d tanZ;
            math::tangentSpace(tanY, &tanX, &tanZ);

            // 计算世界坐标系中射线方向
            Vector3d dirWorld = dirLocal.x * tanX + dirLocal.y * tanY + dirLocal.z * tanZ;

            // 设置 ray
            (*ray) = Ray(_center, NORMALIZE(dirWorld), _mediumBoundary.inside());

            (*normal) = dirWorld;
            (*pdfPos) = 1.0;
            (*pdfDir) = math::sampling::coneUniformSamplePdf(_cosTotalRange);

            return _intensity * fallOffWeight(dirWorld);
        }

        void SpotLight::randomLightRayPdf(const Ray &ray, const Vector3d &,
                                          double *pdfPos, double *pdfDir) const {
            (*pdfPos) = 0;
            (*pdfDir) = std::cos(DOT(ray.getDirection(), _dir)) >= _cosTotalRange ?
                        math::sampling::coneUniformSamplePdf(_cosTotalRange) : 0;
        }

        Spectrum SpotLight::fallOffWeight(const Vector3d &wo) {
            double cosine = DOT(wo, _dir);
            if (cosine < _cosTotalRange) {
                return Spectrum(0);
            } else if (cosine < _cosFallOffRange) {
                double fraction = (cosine - _cosTotalRange) / (_cosFallOffRange - _cosTotalRange);
                return std::pow(fraction, 4);
            } else {
                return Spectrum(1.0);
            }
        }

    }
}