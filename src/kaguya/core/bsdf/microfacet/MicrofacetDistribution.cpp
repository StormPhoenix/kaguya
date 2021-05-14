//
// Created by Storm Phoenix on 2021/5/7.
//

#include <kaguya/core/bsdf/microfacet/MicrofacetDistribution.h>
#include <kaguya/math/Math.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            namespace microfacet {
                using namespace math;

                Float MicrofacetDistribution::G(const Vector3F &v, const Normal3F &wh) const {
                    return 1. / (1. + lambda(v, wh));
                }

                Float MicrofacetDistribution::G(const Vector3F &wo, const Vector3F &wi, const Normal3F &wh) const {
                    return 1. / (1. + lambda(wo, wh) + lambda(wi, wh));
                }

                Float MicrofacetDistribution::samplePdf(const Vector3F &wo, const Vector3F &wh) const {
                    // cos(theta) = shift (D(w_h) * G(w_h, w) * cos(w, w_h)) d_wh
//                return D(wh) * G(wo) * ABS_DOT(wo, wh) / std::abs(wo.y);
                    return D(wh) * std::abs(local_coord::cosTheta(wh));
                }
            }
        }
    }
}