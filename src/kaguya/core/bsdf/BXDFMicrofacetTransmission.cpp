//
// Created by Storm Phoenix on 2021/5/13.
//

#include <kaguya/math/Math.h>
#include <kaguya/core/bsdf/BXDFMicrofacetTransmission.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            BXDFMicrofacetTransmission::BXDFMicrofacetTransmission(Float etaI, Float etaT, const Spectrum &Kt,
                                                                   const MicrofacetDistribution *microfacetDistribution,
                                                                   TransportMode mode) :
                    BXDF(BXDFType(BXDFType::BSDF_TRANSMISSION | BXDFType::BSDF_GLOSSY)), _etaI(etaI), _etaT(etaT),
                    _Kt(Kt), _microfacetDistribution(microfacetDistribution), _mode(mode), _fresnel(etaI, etaT) {}

            Spectrum BXDFMicrofacetTransmission::f(const Vector3F &wo, const Vector3F &wi) const {
                // Check parameter validation
                Float cosThetaWo = math::local_coord::cosTheta(wo);
                Float cosThetaWi = math::local_coord::cosTheta(wi);

                if (cosThetaWo == 0 || cosThetaWi == 0) {
                    return Spectrum(0.0f);
                }

                if (cosThetaWo * cosThetaWi >= 0) {
                    return Spectrum(0.0f);
                }

                // Compute half angle
                Float invEta = cosThetaWo > 0 ? _etaT / _etaI : _etaI / _etaT;
                Normal3F wh = NORMALIZE(wo + invEta * wi);
                // Correction for positive hemisphere
                if (wh.y < 0) {
                    wh *= -1;
                }
                Float eta = 1.0 / invEta;

                // Transmission check
                if (DOT(wo, wh) * DOT(wi, wh) >= 0) {
                    return Spectrum(0.0f);
                }

                Float D_Wh = _microfacetDistribution->D(wh);
                Float G_Wo_Wi = _microfacetDistribution->G(wo, wi, wh);

                Float cosThetaWOH = DOT(wo, wh);
                Spectrum Fr = _fresnel.fresnel(cosThetaWOH);

                Float absCosThetaWIH = ABS_DOT(wi, wh);
                Float absCosThetaWOH = ABS_DOT(wo, wh);

                Float cosThetaWIH = DOT(wi, wh);
                Float term = cosThetaWOH + invEta * (cosThetaWIH);

                Float scale = _mode == TransportMode::RADIANCE ? eta : 1.;

                // TODO different from PBRT
                return _Kt * (Spectrum(1.0) - Fr) *
                       std::abs(((D_Wh * G_Wo_Wi * scale * scale) / (term * term)) *
                                ((absCosThetaWIH * absCosThetaWOH) / (cosThetaWo * cosThetaWi)));

//                return _Kt * (Spectrum(1.0) - Fr) *
//                       std::abs(((D_Wh * G_Wo_Wi * scale * scale * invEta * invEta) / (term * term)) *
//                                ((absCosThetaWIH * absCosThetaWOH) / (cosThetaWo * cosThetaWi)));
            }

            Spectrum BXDFMicrofacetTransmission::sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                                         Sampler *const sampler, BXDFType *sampleType) {
                Float cosThetaWo = math::local_coord::cosTheta(wo);
                if (cosThetaWo == 0) {
                    return Spectrum(0.0f);
                }

                // Sample half angle
                Normal3F wh = _microfacetDistribution->sampleWh(wo, sampler);
                Float cosThetaWOH = DOT(wo, wh);
                if (cosThetaWOH <= 0.) {
                    return Spectrum(0.0f);
                }

                Float eta = cosThetaWo > 0 ? _etaI / _etaT : _etaT / _etaI;
                Float invEta = 1.0 / eta;

                if (!math::refract(wo, wh, eta, wi)) {
                    return Spectrum(0.);
                }
                (*wi) = NORMALIZE(*wi);

                if (pdf != nullptr) {
                    (*pdf) = samplePdf(wo, *wi);
                }

                if (sampleType != nullptr) {
                    (*sampleType) = BXDFType(BXDFType::BSDF_GLOSSY | BXDFType::BSDF_TRANSMISSION);
                }

                return f(wo, *wi);
            }

            Float BXDFMicrofacetTransmission::samplePdf(const Vector3F &wo, const Vector3F &wi) const {
                Float cosThetaWo = math::local_coord::cosTheta(wo);
                Float cosThetaWi = math::local_coord::cosTheta(wi);

                if (cosThetaWo * cosThetaWi > 0) {
                    return 0.0f;
                }

                Float eta = cosThetaWo > 0 ? _etaI / _etaT : _etaT / _etaI;
                Float invEta = 1.0 / eta;

                Normal3F wh = NORMALIZE(wo + invEta * wi);
                // Transmission check
                if (DOT(wo, wh) * DOT(wi, wh) >= 0) {
                    return 0.f;
                }

                // TODO different from PBRT
                Float sqrtDenom = DOT(wo, wh) + invEta * DOT(wi, wh);
                // TODO delete
                return _microfacetDistribution->samplePdf(wo, wh) * ABS_DOT(wi, wh) / (sqrtDenom * sqrtDenom);
//                return _microfacetDistribution->samplePdf(wo, wh) *
//                       std::abs(invEta * invEta * DOT(wi, wh)) /
//                       (sqrtDenom * sqrtDenom);
            }
        }
    }
}