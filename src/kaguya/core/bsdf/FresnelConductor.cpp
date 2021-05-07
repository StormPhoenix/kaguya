//
// Created by Storm Phoenix on 2020/10/15.
//

#include <kaguya/core/bsdf/FresnelConductor.h>

namespace kaguya {
    namespace core {

        Spectrum FresnelConductor::fresnel(Float cosineTheta) const {
            return fresnelConductor(cosineTheta, _thetaI, _thetaT, _k);
        }

    }
}