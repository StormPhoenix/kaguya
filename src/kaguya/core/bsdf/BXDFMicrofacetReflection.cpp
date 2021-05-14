//
// Created by Storm Phoenix on 2021/5/7.
//

#include <kaguya/core/bsdf/BXDFMicrofacetReflection.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            BXDFMicrofacetReflection::BXDFMicrofacetReflection(const Spectrum &Ks,
                                                               const MicrofacetDistribution *microfacetDistribution,
                                                               const Fresnel *fresnel) :
                    BXDF(BXDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
                    _Ks(Ks),
                    _microfacetDistribution(microfacetDistribution),
                    _fresnel(fresnel) {
                ASSERT(_microfacetDistribution != nullptr, "MicrofacetDistribution can't be nullptr. ");
                ASSERT(_fresnel != nullptr, "Fresnel can't be nullptr. ");
            }

            Spectrum BXDFMicrofacetReflection::f(const Vector3F &wo, const Vector3F &wi) const {
                Float cosThetaO = wo.y;
                Float cosThetaI = wi.y;
                if (cosThetaI == 0 || cosThetaO == 0) {
                    return Spectrum(0.0f);
                }

                if (cosThetaI * cosThetaO <= 0) {
                    return Spectrum(0.0f);
                }

                // Half angle
                Vector3F wh = NORMALIZE(wo + wi);
                if (wh.y < 0) {
                    wh *= -1;
                }

                Float D_Wh = _microfacetDistribution->D(wh);
                Float G_Wo_Wi = _microfacetDistribution->G(wo, wi, wh);

                Float cosThetaH = DOT(wi, wh);
                Spectrum Fr = _fresnel->fresnel(cosThetaH);

                return (D_Wh * G_Wo_Wi * Fr * _Ks) / (4 * std::abs(cosThetaO * cosThetaI));
            }

            Spectrum BXDFMicrofacetReflection::sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                                       Sampler *const sampler, BXDFType *sampleType) {
                if (wo.y == 0.) {
                    return 0.;
                }

                Vector3F wh = _microfacetDistribution->sampleWh(wo, sampler);
                if (DOT(wo, wh) < 0) {
                    return 0.;
                }

                // Check same side
                Vector3F reflectDir = math::reflect(wo, wh);

                if ((wo.y * reflectDir.y) <= 0) {
                    if (pdf != nullptr) {
                        (*pdf) = 0;
                    }
                    return 0.;
                }

                if (wi != nullptr) {
                    (*wi) = reflectDir;
                }

                if (pdf != nullptr) {
//                    (*pdf) = _microfacetDistribution->samplePdf(wo, wh) / (4 * DOT(wo, wh));
                    (*pdf) = samplePdf(wo, reflectDir);
                }

                if (sampleType != nullptr) {
                    (*sampleType) = BXDFType(BXDFType::BSDF_GLOSSY | BXDFType::BSDF_REFLECTION);
                }

                return f(wo, reflectDir);
            }

            Float BXDFMicrofacetReflection::samplePdf(const Vector3F &wo, const Vector3F &wi) const {
                // Check same hemisphere
                if (wo.y * wi.y <= 0) {
                    return 0.;
                }
                Vector3F wh = NORMALIZE(wo + wi);
                return _microfacetDistribution->samplePdf(wo, wh) / (4 * DOT(wo, wh));
            }
        }
    }
}