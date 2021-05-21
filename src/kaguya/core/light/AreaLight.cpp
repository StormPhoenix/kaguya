//
// Created by Storm Phoenix on 2020/10/22.
//

#include <kaguya/core/light/AreaLight.h>

namespace RENDER_NAMESPACE {
    namespace core {

        AreaLight::AreaLight(const Spectrum &intensity, std::shared_ptr<Shape> shape, LightType type,
                             const MediumInterface &mediumBoundary) :
                Light(LightType(type | AREA), mediumBoundary), _intensity(intensity), _shape(shape) {}

        Spectrum AreaLight::sampleLi(const Interaction &eye, Vector3F *wi, Float *pdf,
                                     Sampler *sampler,
                                     VisibilityTester *visibilityTester) {
            assert(_shape != nullptr);
            // 从 eye 出发采样一条射线，返回与 shape 的交点
            SurfaceInteraction si = _shape->sampleSurfaceInteraction(eye, pdf, sampler);
            (*wi) = NORMALIZE(si.point - eye.point);
            // Visibility check
            (*visibilityTester) = VisibilityTester(eye, si);
            return L(si, -(*wi));
        }

        Float AreaLight::pdfLi(const Interaction &eye, const Vector3F &dir) {
            assert(_shape != nullptr);
            return _shape->surfaceInteractionPdf(eye, dir);
        }


    }
}