//
// Created by Storm Phoenix on 2020/10/22.
//

#include <kaguya/core/light/AreaLight.h>

namespace kaguya {
    namespace core {

        AreaLight::AreaLight(const Spectrum &intensity, std::shared_ptr<Geometry> shape, LightType type,
                             const MediumBoundary &mediumBoundary) :
                Light(LightType(type | AREA), mediumBoundary), _intensity(intensity), _geometry(shape) {}

        Spectrum AreaLight::sampleLi(const Interaction &eye, Vector3F *wi, Float *pdf,
                                     Sampler *sampler,
                                     VisibilityTester *visibilityTester) {
            assert(_geometry != nullptr);
            // 从 eye 出发采样一条射线，返回与 shape 的交点
            SurfaceInteraction si = _geometry->getShape()->sampleSurfaceInteraction(eye, sampler);
            // 射线方向
            (*wi) = NORMALIZE(si.point - eye.point);
            // 该射线方向的 PDF
            (*pdf) = _geometry->getShape()->surfaceInteractionPdf(eye, *wi);
            // 可见性判断
            (*visibilityTester) = VisibilityTester(eye, si);
            // 计算返回的 Spectrum
            return L(si, -(*wi));
        }

        Float AreaLight::pdfLi(const Interaction &eye, const Vector3F &dir) {
            assert(_geometry != nullptr);
            return _geometry->getShape()->surfaceInteractionPdf(eye, dir);
        }


    }
}