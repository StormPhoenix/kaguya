//
// Created by Storm Phoenix on 2020/10/22.
//

#include <kaguya/core/light/AreaLight.h>

namespace kaguya {
    namespace core {

        AreaLight::AreaLight(const Spectrum &intensity, std::shared_ptr<ShapeSampler> shapeSampler, LightType type) :
                Light(LightType(type | AREA)), _intensity(intensity), _shapeSampler(shapeSampler) {}

        Spectrum AreaLight::sampleRay(const Interaction &eye, Vector3 *wi, double *pdf,
                                      VisibilityTester *visibilityTester) {
            assert(_shapeSampler != nullptr);
            // 从 eye 出发采样一条射线，返回与 shape 的交点
            Interaction intersection = _shapeSampler->sampleRayIntersection(eye);
            // 射线方向
            (*wi) = NORMALIZE(intersection.point - eye.point);
            // 该射线方向的 PDF
            (*pdf) = _shapeSampler->rayPdf(eye, *wi);
            // 可见性判断
            (*visibilityTester) = VisibilityTester(eye, intersection);
            // 计算返回的 Spectrum
            return luminance(intersection, -(*wi));
        }

        double AreaLight::sampleRayPdf(const Interaction &eye, const Vector3 &dir) {
            assert(_shapeSampler != nullptr);
            return _shapeSampler->rayPdf(eye, dir);
        }

    }
}