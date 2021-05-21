//
// Created by Storm Phoenix on 2020/10/15.
//

#ifndef KAGUYA_FRESNELCONDUCTOR_H
#define KAGUYA_FRESNELCONDUCTOR_H

#include <kaguya/core/bsdf/fresnel/Fresnel.h>

namespace RENDER_NAMESPACE {
    namespace core {
        namespace bsdf {
            namespace fresnel {
                class FresnelConductor : public Fresnel {
                public:
                    FresnelConductor(const Spectrum &thetaI,
                                     const Spectrum &thetaT,
                                     const Spectrum &k) :
                            _thetaI(thetaI), _thetaT(thetaT), _k(k) {}

                    virtual Spectrum fresnel(Float cosineTheta) const override;

                private:
                    Spectrum _thetaI;
                    Spectrum _thetaT;
                    Spectrum _k;
                };
            }
        }
    }
}

#endif //KAGUYA_FRESNELCONDUCTOR_H
