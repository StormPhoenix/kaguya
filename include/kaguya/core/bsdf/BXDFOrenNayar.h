//
// Created by Storm Phoenix on 2021/5/7.
//

#ifndef KAGUYA_BXDFORENNAYAR_H
#define KAGUYA_BXDFORENNAYAR_H

#include <kaguya/core/bsdf/BXDF.h>

namespace kaguya {
    namespace core {

        class BXDFOrenNayar : public BXDF {
        public:
            BXDFOrenNayar(const Spectrum &reflectance, Float sigma = 0.);

            virtual Spectrum f(const Vector3F &wo, const Vector3F &wi) const override;

        private:
            const Spectrum _reflectance;
            const Float _sigma;
        };

    }
}

#endif //KAGUYA_BXDFORENNAYAR_H
