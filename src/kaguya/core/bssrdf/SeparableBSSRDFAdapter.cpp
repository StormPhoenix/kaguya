//
// Created by Storm Phoenix on 2021/1/11.
//

#include <kaguya/core/bssrdf/SeparableBSSRDF.h>
#include <kaguya/core/bssrdf/SeparableBSSRDFAdapter.h>

namespace kaguya {
    namespace core {
        namespace bssrdf {

            using bsdf::BXDFType;

            SeparableBSSRDFAdapter::SeparableBSSRDFAdapter(const SeparableBSSRDF *bssrdf) :
                    BXDF(BXDFType(BXDFType::BSDF_REFLECTION | BXDFType::BSDF_DIFFUSE)), _bssrdf(bssrdf) {
                assert(_bssrdf != nullptr);
            }

            Spectrum SeparableBSSRDFAdapter::f(const Vector3F &wo, const Vector3F &wi) const {
                // TODO check radiance transport mode
                return _bssrdf->subsurfaceWi(wi);
            }
        }
    }
}