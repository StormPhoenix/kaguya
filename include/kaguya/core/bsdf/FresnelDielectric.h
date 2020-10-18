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
            FresnelDielectric(double thetaI, double thetaT) :
                    _thetaI(thetaI), _thetaT(thetaT) {}

            virtual Spectrum fresnel(double cosineTheta) override;

        private:
            double _thetaI;
            double _thetaT;
        };

    }
}

#endif //KAGUYA_FRESNELDIELECTRIC_H