//
// Created by Storm Phoenix on 2021/5/10.
//

#include <kaguya/core/bsdf/BXDFFresnelMicrofacetReflection.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            Spectrum fresnelSchlick(Float cosTheta, const Spectrum &Rs) {
                auto pow5Func = [](Float x) -> Float { return x * x * x * x * x; };
                return Rs + (Spectrum(1.) - Rs) * pow5Func(1 - cosTheta);
            }

            BXDFFresnelMicrofacetReflection::BXDFFresnelMicrofacetReflection(const Spectrum &Rd, const Spectrum &Rs,
                                                                             const MicrofacetDistribution *dist) :
                    BXDF(BXDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
                    _Rd(Rd), _Rs(Rs), _distribution(dist) {}

            Spectrum BXDFFresnelMicrofacetReflection::f(const Vector3F &wo, const Vector3F &wi) const {
                Float absCosThetaWo = math::local_coord::absCosTheta(wo);
                Float absCosThetaWi = math::local_coord::absCosTheta(wi);
                auto pow5Func = [](Float x) -> Float { return x * x * x * x * x; };

                // Diffuse
                Spectrum diffuse = (28.f * _Rd / (23.f * math::PI)) * (Spectrum(1.0) - _Rs) *
                                   (1 - pow5Func(1 - (absCosThetaWi * 0.5))) *
                                   (1 - pow5Func(1 - (absCosThetaWo * 0.5)));

                Vector3F wh = (wo + wi) * 0.5f;
                if (wh.x == 0. || wh.y == 0. || wh.z == 0.) {
                    return 0.;
                }
                wh = NORMALIZE(wh);

                // Specular
                Spectrum specular = (_distribution->D(wh) * fresnelSchlick(DOT(wo, wh), _Rs)) /
                                    (4 * (ABS_DOT(wh, wi)) * std::max(absCosThetaWi, absCosThetaWo));
                return diffuse + specular;
            }

            Spectrum BXDFFresnelMicrofacetReflection::sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                                              Sampler *const sampler, BXDFType *sampleType) {
                Float u = sampler->sample1D();
                if (u < 0.5f) {
                    // Sample diffuse
                    (*wi) = math::sampling::hemiCosineSampling(sampler);
                    if (wo.y < 0) {
                        wi->y *= -1;
                    }
                } else {
                    // Sample specular
                    Vector3F wh = _distribution->sampleWh(wo, sampler);
                    (*wi) = math::reflect(wo, wh);
                    if (wo.y * wi->y < 0) {
                        return 0.;
                    }
                }
                if (pdf != nullptr) {
                    (*pdf) = samplePdf(wo, *wi);
                }
                return f(wo, *wi);
            }

            Float BXDFFresnelMicrofacetReflection::samplePdf(const Vector3F &wo, const Vector3F &wi) const {
                if (wo.y * wi.y < 0) {
                    return 0;
                }

                // Diffuse reflection pdf
                Float pdfDiffuse  = 0.5 * math::local_coord::absCosTheta(wi) * math::INV_PI;

                // Specular reflection pdf
                Vector3F wh = NORMALIZE(wo + wi);
                Float pdfSpecular = _distribution->samplePdf(wo, wh) / (4 * DOT(wo, wh));
                return pdfDiffuse + pdfSpecular;
            }
        }
    }
}