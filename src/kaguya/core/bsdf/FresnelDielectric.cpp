//
// Created by Storm Phoenix on 2020/10/15.
//

#include <kaguya/core/bsdf/FresnelDielectric.h>

namespace kaguya {
    namespace core {

        Spectrum FresnelDielectric::fresnel(double cosineTheta) {
            return fresnelDielectric(cosineTheta, _thetaI, _thetaT);
        }

    }
}