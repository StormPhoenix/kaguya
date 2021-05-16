//
// Created by Storm Phoenix on 2021/5/7.
//

#ifndef KAGUYA_BXDFORENNAYAR_H
#define KAGUYA_BXDFORENNAYAR_H

#include <kaguya/core/bsdf/BXDF.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            class BXDFOrenNayar : public BXDF {
            public:
                BXDFOrenNayar(const Spectrum &reflectance, Float roughness = 0.);

                virtual Spectrum f(const Vector3F &wo, const Vector3F &wi) const override;

            private:
                Float A, B;
                const Spectrum _Kd;
            };
        }
    }
}

#endif //KAGUYA_BXDFORENNAYAR_H
