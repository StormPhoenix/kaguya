//
// Created by Storm Phoenix on 2020/10/15.
//

#include <kaguya/core/bsdf/fresnel/FresnelConductor.h>

namespace RENDER_NAMESPACE {
    namespace core {
        namespace bsdf {
            namespace fresnel {

                Spectrum FresnelConductor::fresnel(Float cosineTheta) const {
                    return fresnelConductor(cosineTheta, _thetaI, _thetaT, _k);
                }
            }
        }
    }
}