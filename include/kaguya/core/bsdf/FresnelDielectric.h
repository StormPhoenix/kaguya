//
// Created by Storm Phoenix on 2020/10/15.
//

#ifndef KAGUYA_FRESNELDIELECTRIC_H
#define KAGUYA_FRESNELDIELECTRIC_H

#include <kaguya/core/bsdf/Fresnel.h>

namespace kaguya {
    namespace core {

        class FresnelDielectric : public Fresnel {
        public:
            FresnelDielectric(Float thetaI, Float thetaT) :
                    _thetaI(thetaI), _thetaT(thetaT) {}

            virtual Spectrum fresnel(Float cosineTheta) const override;

        private:
            Float _thetaI;
            Float _thetaT;
        };

    }
}

#endif //KAGUYA_FRESNELDIELECTRIC_H
