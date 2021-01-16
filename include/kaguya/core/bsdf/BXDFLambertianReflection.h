//
// Created by Storm Phoenix on 2020/10/15.
//

#ifndef KAGUYA_BXDFLAMBERTIANREFLECTION_H
#define KAGUYA_BXDFLAMBERTIANREFLECTION_H

#include <kaguya/core/bsdf/BXDF.h>

namespace kaguya {
    namespace core {

        class BXDFLambertianReflection : public BXDF {
        public:
            BXDFLambertianReflection(const Spectrum &albedo);

            virtual Spectrum f(const Vector3F &wo, const Vector3F &wi) const override;

        private:
            const Spectrum _albedo;
        };

    }
}

#endif //KAGUYA_BXDFLAMBERTIANREFLECTION_H
