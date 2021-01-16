//
// Created by Storm Phoenix on 2020/10/25.
//

#include <kaguya/core/light/SpotLight.h>

namespace kaguya {
    namespace core {

        SpotLight::SpotLight(const Vector3F center, const Vector3F dir, Spectrum intensity,
                             const MediumBound &mediumBoundary, Float fallOffRange, Float totalRange) :
                Light(DELTA_POSITION, mediumBoundary),
                _center(center), _dir(NORMALIZE(dir)),
                _intensity(intensity) {
            assert(_mediumBoundary.inside() == _mediumBoundary.outside());

            _cosFallOffRange = std::cos(math::DEGREES_TO_RADIANS(fallOffRange));
            _cosTotalRange = std::cos(math::DEGREES_TO_RADIANS(totalRange));
        }

        Spectrum SpotLight::sampleFromLight(const Interaction &eye,
                                            Vector3F *wi, Float *pdf,
                                            Sampler *sampler1D,
                                            VisibilityTester *visibilityTester) {
            (*wi) = NORMALIZE(_center - eye.point);
            (*pdf) = 1.0;

            Vector3F samplePoint = _center;
            Vector3F sampleDir = NORMALIZE(_center - eye.point);
            Vector3F sampleNormal = -sampleDir;
            Interaction interaction = Interaction(samplePoint, sampleDir, sampleNormal, _mediumBoundary);

            (*visibilityTester) = VisibilityTester(eye, interaction);
            return _intensity * fallOffWeight(-(*wi)) / std::pow(LENGTH(_center - eye.point), 2);
        }

        Float SpotLight::sampleFromLightPdf(const Interaction &eye, const Vector3F &dir) {
            return 0;
        }

        Spectrum SpotLight::randomLightRay(Ray *ray, Vector3F *normal, Float *pdfPos, Float *pdfDir,
                                           Sampler *sampler1D) {
            // 在局部坐标空间中均匀采样射线
            Vector3F dirLocal = math::sampling::coneUniformSampling(_cosTotalRange, sampler1D);

            // 计算切线空间
            Vector3F tanY = _dir;
            Vector3F tanX;
            Vector3F tanZ;
            math::tangentSpace(tanY, &tanX, &tanZ);

            // 计算世界坐标系中射线方向
            Vector3F dirWorld = dirLocal.x * tanX + dirLocal.y * tanY + dirLocal.z * tanZ;

            // 设置 ray
            (*ray) = Ray(_center, NORMALIZE(dirWorld), _mediumBoundary.inside());

            (*normal) = dirWorld;
            (*pdfPos) = 1.0;
            (*pdfDir) = math::sampling::coneUniformSamplePdf(_cosTotalRange);

            return _intensity * fallOffWeight(dirWorld);
        }

        void SpotLight::randomLightRayPdf(const Ray &ray, const Vector3F &,
                                          Float *pdfPos, Float *pdfDir) const {
            (*pdfPos) = 0;
            (*pdfDir) = std::cos(DOT(ray.getDirection(), _dir)) >= _cosTotalRange ?
                        math::sampling::coneUniformSamplePdf(_cosTotalRange) : 0;
        }

        Spectrum SpotLight::fallOffWeight(const Vector3F &wo) {
            Float cosine = DOT(wo, _dir);
            if (cosine < _cosTotalRange) {
                return Spectrum(0);
            } else if (cosine < _cosFallOffRange) {
                Float fraction = (cosine - _cosTotalRange) / (_cosFallOffRange - _cosTotalRange);
                return std::pow(fraction, 4);
            } else {
                return Spectrum(1.0);
            }
        }

    }
}