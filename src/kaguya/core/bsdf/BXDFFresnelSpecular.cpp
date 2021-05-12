//
// Created by Storm Phoenix on 2020/10/16.
//

#include <kaguya/sampler/Sampler.h>
#include <kaguya/core/bsdf/BXDFFresnelSpecular.h>
#include <kaguya/core/bsdf/fresnel/Fresnel.h>
#include <kaguya/math/Math.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            BXDFFresnelSpecular::BXDFFresnelSpecular(const Spectrum &reflectance, const Spectrum &transmittance,
                                                     Float thetaI, Float thetaT, TransportMode mode) :
                    BXDF(BXDFType(BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION)),
                    _reflectance(reflectance), _transmittance(transmittance),
                    _thetaI(thetaI), _thetaT(thetaT), _mode(mode) {}

            Spectrum BXDFFresnelSpecular::f(const Vector3F &wo, const Vector3F &wi) const {
                return Spectrum(0.0);
            }

            Spectrum BXDFFresnelSpecular::sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                                  Sampler *const sampler, BXDFType *sampleType) {
                Float cosine = wo.y;
                // 计算反射概率
                // TODO move reflect probabilty computation to fresnel
                Float reflectProb = fresnel::fresnelDielectric(cosine, _thetaI, _thetaT);
                // Fresnel 的近似计算
//            Float reflectProb = math::schlick(cosine, _thetaI / _thetaT);

                Float random = sampler->sample1D();
                if (random < reflectProb) {
                    // Reflection
                    if (sampleType != nullptr) {
                        *sampleType = BXDFType(BSDF_SPECULAR | BSDF_REFLECTION);
                    }
                    // 反射
                    *wi = Vector3F(-wo.x, wo.y, -wo.z);
                    *pdf = reflectProb;
                    // f(p, w_o, w_i) / cos(theta(w_i))
                    if ((*wi).y == 0) {
                        return 0;
                    }
                    return reflectProb * _reflectance / abs(wi->y);
                } else {
                    // Refraction
                    Vector3F normal = Vector3F(0.0, 1.0, 0.0);
                    Float refraction;
                    if (wo.y > 0) {
                        // Travel from outer
                        refraction = _thetaI / _thetaT;
                    } else {
                        // Travel from inner
                        refraction = _thetaT / _thetaI;
                        normal.y *= -1;
                    }

                    if (!math::refract(wo, normal, refraction, wi)) {
                        // Totally reflection
                        return 0;
                    }

                    *pdf = 1 - reflectProb;
                    Spectrum f = _transmittance * (1.0 - reflectProb) / std::abs(wi->y);
                    if (_mode == RADIANCE) {
                        f *= std::pow(refraction, 2);
                    }
                    // 设置散射类型
                    if (sampleType != nullptr) {
                        *sampleType = BXDFType(BSDF_SPECULAR | BSDF_TRANSMISSION);
                    }
                    return f;
                }
            }

            Float BXDFFresnelSpecular::samplePdf(const Vector3F &wo, const Vector3F &wi) const {
                return 0.0;
            }
        }
    }
}