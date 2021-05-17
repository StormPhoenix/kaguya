//
// Created by Storm Phoenix on 2021/5/18.
//

#include <kaguya/core/light/DirectLight.h>

namespace RENDER_NAMESPACE {
    namespace core {
        Spectrum DirectLight::sampleLi(const Interaction &eye, Vector3F *wi, Float *pdf,
                                       Sampler *sampler, VisibilityTester *visibilityTester) {
            if (wi != nullptr) {
                (*wi) = -_direction;
            }

            if (pdf != nullptr) {
                (*pdf) = 1.0;
            }

            if (visibilityTester != nullptr) {
                Point3F infinitePoint = eye.point + (-_direction * Float(2.0 * _worldRadius));
                (*visibilityTester) = VisibilityTester(eye, Interaction(infinitePoint, -_direction, _direction,
                                                                        _mediumInterface));
            }
            return L;
        }

        Float DirectLight::pdfLi(const Interaction &eye, const Vector3F &dir) {
            return 0.0f;
        }
    }
}