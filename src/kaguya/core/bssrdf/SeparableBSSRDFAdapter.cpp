//
// Created by Storm Phoenix on 2021/1/11.
//

#include <kaguya/core/bssrdf/SeparableBSSRDF.h>
#include <kaguya/core/bssrdf/SeparableBSSRDFAdapter.h>

namespace kaguya {
    namespace core {

        SeparableBSSRDFAdapter::SeparableBSSRDFAdapter(const SeparableBSSRDF *bssrdf) :
                BXDF(BXDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), _bssrdf(bssrdf) {
            assert(_bssrdf != nullptr);
        }

        Spectrum SeparableBSSRDFAdapter::f(const Vector3d &wo, const Vector3d &wi) const {
            // TODO check radiance transport mode
            return _bssrdf->subsurfaceWi(wi);
        }
    }
}