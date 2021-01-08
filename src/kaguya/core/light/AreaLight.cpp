//
// Created by Storm Phoenix on 2020/10/22.
//

#include <kaguya/core/light/AreaLight.h>

namespace kaguya {
    namespace core {

        AreaLight::AreaLight(const Spectrum &intensity, std::shared_ptr<Geometry> shape, LightType type,
                             const MediumBound &mediumBoundary) :
                Light(LightType(type | AREA), mediumBoundary), _intensity(intensity), _geometry(shape) {}

        Spectrum AreaLight::sampleFromLight(const Interaction &eye, Vector3d *wi, double *pdf,
                                            Sampler *sampler1D,
                                            VisibilityTester *visibilityTester) {
            assert(_geometry != nullptr);
            // 从 eye 出发采样一条射线，返回与 shape 的交点
            SurfaceInteraction intersection = _geometry->getShape()->sampleSurfaceInteraction(eye, sampler1D);
            // 射线方向
            (*wi) = NORMALIZE(intersection.getPoint() - eye.getPoint());
            // 该射线方向的 PDF
            (*pdf) = _geometry->getShape()->surfaceInteractionPdf(eye, *wi);
            // 可见性判断
            (*visibilityTester) = VisibilityTester(eye, intersection);
            // 计算返回的 Spectrum
            return lightRadiance(intersection, -(*wi));
        }

        double AreaLight::sampleFromLightPdf(const Interaction &eye, const Vector3d &dir) {
            assert(_geometry != nullptr);
            return _geometry->getShape()->surfaceInteractionPdf(eye, dir);
        }


    }
}