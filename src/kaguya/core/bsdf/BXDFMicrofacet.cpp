//
// Created by Storm Phoenix on 2021/5/14.
//

#include <kaguya/math/Math.h>
#include <kaguya/core/bsdf/fresnel/Fresnel.h>
#include <kaguya/core/bsdf/BXDFMicrofacet.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            BXDFMicrofacet::BXDFMicrofacet(const Spectrum &Ks, const Spectrum &Kt, Float etaI, Float etaT,
                                           const MicrofacetDistribution *distribution, const TransportMode mode) :
                    BXDF(BXDFType(BXDFType::BSDF_GLOSSY | BXDFType::BSDF_TRANSMISSION | BXDFType::BSDF_REFLECTION)),
                    _Ks(Ks), _Kt(Kt), _etaI(etaI), _etaT(etaT), _microfacetDistribution(distribution), _mode(mode) {}

            Spectrum BXDFMicrofacet::sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                             Sampler *const sampler, BXDFType *sampleType) {
                Float cosThetaWo = math::local_coord::cosTheta(wo);
                if (cosThetaWo == 0.0) {
                    return Spectrum(0.f);
                }

                // Sample microfacet
                Normal3F wh = _microfacetDistribution->sampleWh(wo, sampler);
                Float cosThetaWoWh = DOT(wo, wh);
                if (cosThetaWoWh <= 0.0f) {
                    // Back face
                    return Spectrum(0.f);
                }

                // Reflection probability computation
                Float reflectionProb = fresnel::fresnelDielectric(cosThetaWoWh,
                                                                  cosThetaWo > 0 ? _etaI : _etaT,
                                                                  cosThetaWo > 0 ? _etaT : _etaI);

                Float scatterSample = sampler->sample1D();
                if (scatterSample < reflectionProb) {
                    // Reflection
                    Vector3F reflectDir = math::reflect(wo, wh);
                    if (wo.y * reflectDir.y < 0) {
                        if (pdf != nullptr) {
                            (*pdf) = 0.0;
                        }
                        return 0.f;
                    }

                    if (wi != nullptr) {
                        (*wi) = reflectDir;
                    }

                    if (pdf != nullptr) {
                        (*pdf) = samplePdf(wo, reflectDir);
                    }

                    if (sampleType != nullptr) {
                        (*sampleType) = BXDFType(BXDFType::BSDF_GLOSSY | BXDFType::BSDF_REFLECTION);
                    }
                    return f(wo, reflectDir);
                } else {
                    // Refraction
                    Vector3F refractDir(0);
                    Float eta = cosThetaWo > 0 ? _etaI / _etaT : _etaT / _etaI;
                    if (!math::refract(wo, wh, eta, &refractDir)) {
                        // Refraction failed
                        return Spectrum(0.);
                    }

                    refractDir = NORMALIZE(refractDir);
                    if (refractDir.y * wo.y > 0) {
                        if (pdf != nullptr) {
                            (*pdf) = 0.f;
                        }
                        return Spectrum(0.0f);
                    }

                    if (wi != nullptr) {
                        (*wi) = refractDir;
                    }

                    if (pdf != nullptr) {
                        (*pdf) = samplePdf(wo, refractDir);
                    }

                    if (sampleType != nullptr) {
                        (*sampleType) = BXDFType(BXDFType::BSDF_GLOSSY | BXDFType::BSDF_TRANSMISSION);
                    }
                    return f(wo, refractDir);
                }
            }

            Spectrum BXDFMicrofacet::f(const Vector3F &wo, const Vector3F &wi) const {
                Float cosThetaWo = math::local_coord::cosTheta(wo);
                Float cosThetaWi = math::local_coord::cosTheta(wi);

                if (cosThetaWo == 0 || cosThetaWi == 0) {
                    return Spectrum(0.0f);
                }

                if (cosThetaWo * cosThetaWi < 0) {
                    // Refraction
                    // Compute half angle
                    Float invEta = cosThetaWo > 0 ? _etaT / _etaI : _etaI / _etaT;
                    Normal3F wh = NORMALIZE(wo + invEta * wi);
                    // Correction for positive hemisphere
                    if (wh.y < 0) {
                        wh *= -1;
                    }

                    if (DOT(wo, wh) * DOT(wi, wh) > 0) {
                        return Spectrum(0.0f);
                    }

                    Float D_Wh = _microfacetDistribution->D(wh);
                    Float G_Wo_Wi = _microfacetDistribution->G(wo, wi, wh);

                    Float cosThetaWoWh = DOT(wo, wh);
                    Float Fr = fresnel::fresnelDielectric(cosThetaWoWh, _etaI, _etaT);

                    Float absCosThetaWiWh = ABS_DOT(wi, wh);
                    Float absCosThetaWoWh = ABS_DOT(wo, wh);

                    Float cosThetaWiWh = DOT(wi, wh);
                    Float term = cosThetaWoWh + invEta * (cosThetaWiWh);

                    Float scale = _mode == TransportMode::RADIANCE ? 1.0 / invEta : 1.;
                    return _Kt * (1.0f - Fr) *
                           std::abs(((D_Wh * G_Wo_Wi * scale * scale) / (term * term)) *
                                    ((absCosThetaWiWh * absCosThetaWoWh) / (cosThetaWo * cosThetaWi)));

                } else {
                    // Reflection
                    Vector3F wh = NORMALIZE(wo + wi);
                    if (wh.y < 0) {
                        wh *= -1;
                    }

                    Float D_Wh = _microfacetDistribution->D(wh);
                    Float G_Wo_Wi = _microfacetDistribution->G(wo, wi, wh);

                    Float cosThetaWoWh = DOT(wo, wh);
                    Float Fr = fresnel::fresnelDielectric(cosThetaWoWh, _etaI, _etaT);

                    return (D_Wh * G_Wo_Wi * Fr * _Ks) / (4 * std::abs(cosThetaWi * cosThetaWo));
                }
            }

            Float BXDFMicrofacet::samplePdf(const Vector3F &wo, const Vector3F &wi) const {
                Float cosThetaWo = math::local_coord::cosTheta(wo);
                Float cosThetaWi = math::local_coord::cosTheta(wi);

                if (cosThetaWi == 0 || cosThetaWo == 0) {
                    return 0.0f;
                }

                if (cosThetaWi * cosThetaWo > 0) {
                    // Reflection
                    Vector3F wh = NORMALIZE(wo + wi);
                    Float reflectionProb = fresnel::fresnelDielectric(ABS_DOT(wo, wh),
                                                                      cosThetaWo > 0 ? _etaI : _etaT,
                                                                      cosThetaWi > 0 ? _etaT : _etaI);
                    return reflectionProb * _microfacetDistribution->samplePdf(wo, wh) / (4 * ABS_DOT(wo, wh));
                } else {
                    // Refraction
                    Float eta = cosThetaWo > 0 ? _etaI / _etaT : _etaT / _etaI;
                    Float invEta = 1.0 / eta;

                    Normal3F wh = NORMALIZE(wo + invEta * wi);
                    // Transmission check
                    if (DOT(wo, wh) * DOT(wi, wh) >= 0) {
                        return 0.f;
                    }

                    Float refractionProb = 1.0 - fresnel::fresnelDielectric(ABS_DOT(wo, wh),
                                                                            cosThetaWo > 0 ? _etaI : _etaT,
                                                                            cosThetaWi > 0 ? _etaT : _etaI);
                    Float sqrtDenom = DOT(wo, wh) + invEta * DOT(wi, wh);
                    // Some difference from PBRT
                    return refractionProb * _microfacetDistribution->samplePdf(wo, wh) * ABS_DOT(wi, wh) /
                           (sqrtDenom * sqrtDenom);
                }
            }
        }
    }
}