//
// Created by Storm Phoenix on 2020/10/15.
//

#include <kaguya/core/bsdf/FresnelDielectric.h>

namespace kaguya {
    namespace core {

        Spectrum FresnelDielectric::fresnel(Float cosineTheta) const {
            return math::fresnelDielectric(cosineTheta, _thetaI, _thetaT);
        }

    }
}