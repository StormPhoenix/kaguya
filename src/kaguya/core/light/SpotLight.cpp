//
// Created by Storm Phoenix on 2020/10/25.
//

#include <kaguya/core/light/SpotLight.h>

namespace kaguya {
    namespace core {

        SpotLight::SpotLight(const Vector3 center, const Vector3 dir, Spectrum intensity,
                             double fallOffRange, double totalRange) :
                Light(DELTA_POSITION),
                _center(center), _dir(NORMALIZE(dir)),
                _intensity(intensity) {
            _cosFallOffRange = std::cos(DEGREES_TO_RADIANS(fallOffRange));
            _cosTotalRange = std::cos(DEGREES_TO_RADIANS(totalRange));
        }

        Spectrum SpotLight::sampleRay(const Interaction &eye,
                                      Vector3 *wi, double *pdf,
                                      VisibilityTester *visibilityTester) {
            (*wi) = NORMALIZE(_center - eye.point);
            (*pdf) = 1.0;
            Interaction interaction;
            interaction.point = _center;
            (*visibilityTester) = VisibilityTester(eye, interaction);
            return _intensity * fallOffWeight(-(*wi)) / std::pow(LENGTH(_center - eye.point), 2);
        }

        double SpotLight::sampleRayPdf(const Interaction &eye, const Vector3 &dir) {
            return 0;
        }

        Spectrum SpotLight::sampleLightRay(Ray *ray, Vector3 *normal, double *pdfPos, double *pdfDir) {
            // 在局部坐标空间中均匀采样射线
            Vector3 dirLocal = coneUniformSampling(_cosTotalRange);

            // 计算切线空间
            Vector3 tanY = _dir;
            Vector3 tanX;
            Vector3 tanZ;
            tangentSpace(tanY, &tanX, &tanZ);

            // 计算世界坐标系中射线方向
            Vector3 dirWorld = dirLocal.x * tanX + dirLocal.y * tanY + dirLocal.z * tanZ;

            // 设置 ray
            ray->setOrigin(_center);
            ray->setDirection(dirWorld);

            (*normal) = dirWorld;
            (*pdfPos) = 1.0;
            (*pdfDir) = coneUniformSamplePdf(_cosTotalRange);

            return _intensity * fallOffWeight(dirWorld);
        }

        Spectrum SpotLight::fallOffWeight(const Vector3 &wo) {
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