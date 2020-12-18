//
// Created by Storm Phoenix on 2020/10/22.
//

#include <kaguya/core/light/AreaLight.h>

namespace kaguya {
    namespace core {

        AreaLight::AreaLight(const Spectrum &intensity, std::shared_ptr<Shape> shape, LightType type,
                             const MediumBound &mediumBoundary) :
                Light(LightType(type | AREA), mediumBoundary), _intensity(intensity), _shape(shape) {}

        Spectrum AreaLight::sampleFromLight(const Interaction &eye, Vector3 *wi, double *pdf,
                                            const Sampler1D *sampler1D,
                                            VisibilityTester *visibilityTester) {
            assert(_shape != nullptr);
            // 从 eye 出发采样一条射线，返回与 shape 的交点
            SurfaceInteraction intersection = _shape->sampleSurfaceInteraction(eye, sampler1D);
            // 射线方向
            (*wi) = NORMALIZE(intersection.getPoint() - eye.getPoint());
            // 该射线方向的 PDF
            (*pdf) = _shape->surfaceInteractionPdf(eye, *wi);
            // 可见性判断
            (*visibilityTester) = VisibilityTester(eye, intersection);
            // 计算返回的 Spectrum
            return lightRadiance(intersection, -(*wi));
        }

        double AreaLight::sampleFromLightPdf(const Interaction &eye, const Vector3 &dir) {
            assert(_shape != nullptr);
            return _shape->surfaceInteractionPdf(eye, dir);
        }


    }
}