//
// Created by Storm Phoenix on 2021/1/10.
//

#ifndef KAGUYA_SEPARABLEBSSRDFADAPTER_H
#define KAGUYA_SEPARABLEBSSRDFADAPTER_H

#include <kaguya/core/bsdf/BXDF.h>

namespace RENDER_NAMESPACE {
    namespace core {
        namespace bssrdf {
            using bsdf::BXDF;

            class SeparableBSSRDF;
            class SeparableBSSRDFAdapter : public BXDF {
            public:
                SeparableBSSRDFAdapter(const SeparableBSSRDF *bssrdf);

                virtual Spectrum f(const Vector3F &wo, const Vector3F &wi) const override;

            private:
                const SeparableBSSRDF *_bssrdf;
            };
        }
    }
}

#endif //KAGUYA_SEPARABLEBSSRDFADAPTER_H
