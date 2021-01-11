//
// Created by Storm Phoenix on 2021/1/10.
//

#ifndef KAGUYA_SEPARABLEBSSRDFADAPTER_H
#define KAGUYA_SEPARABLEBSSRDFADAPTER_H

#include <kaguya/core/bsdf/BXDF.h>

namespace kaguya {
    namespace core {

        class SeparableBSSRDF;

        class SeparableBSSRDFAdapter : public BXDF {
        public:
            SeparableBSSRDFAdapter(const SeparableBSSRDF *bssrdf);

            virtual Spectrum f(const Vector3d &wo, const Vector3d &wi) const override;

        private:
            const SeparableBSSRDF *_bssrdf;
        };

    }
}

#endif //KAGUYA_SEPARABLEBSSRDFADAPTER_H
