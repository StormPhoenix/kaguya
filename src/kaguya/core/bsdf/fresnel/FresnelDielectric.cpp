//
// Created by Storm Phoenix on 2020/10/15.
//

#include <kaguya/core/bsdf/fresnel/FresnelDielectric.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            namespace fresnel {
                Spectrum FresnelDielectric::fresnel(Float cosineTheta) const {
                    return fresnelDielectric(cosineTheta, _thetaI, _thetaT);
                }
            }
        }
    }
}