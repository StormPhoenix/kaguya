//
// Created by Storm Phoenix on 2020/10/16.
//

#include <kaguya/sampler/Sampler.h>
#include <kaguya/core/bsdf/BXDFSpecular.h>
#include <kaguya/core/bsdf/fresnel/Fresnel.h>
#include <kaguya/math/Math.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            // TODO albedo -> Reflectance and Transmittance
            BXDFSpecular::BXDFSpecular(const Spectrum &albedo, Float thetaI, Float thetaT, TransportMode mode) :
                    BXDF(BXDFType(BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION)),
                    _albedo(albedo), _thetaI(thetaI), _thetaT(thetaT), _mode(mode) {}

            Spectrum BXDFSpecular::f(const Vector3F &wo, const Vector3F &wi) const {
                return Spectrum(0.0);
            }

            Spectrum BXDFSpecular::sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                           Sampler *const sampler, BXDFType *sampleType) {
                Float cosine = wo.y;
                // 计算反射概率
                // TODO move reflect probabilty computation to fresnel
                Float reflectProb = math::fresnelDielectric(cosine, _thetaI, _thetaT);
                // Fresnel 的近似计算
//            Float reflectProb = math::schlick(cosine, _thetaI / _thetaT);

                // 随机采样是否反射
                Float random = sampler->sample1D();
                if (random < reflectProb) {
                    // 设置散射类型
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
                    return reflectProb * _albedo / abs(wi->y);
                } else {
                    // 折射
                    Float refraction;
                    // 法向
                    Vector3F normal = Vector3F(0.0, 1.0, 0.0);
                    // 判断射入方向
                    if (wo.y > 0) {
                        // 外部射入
                        refraction = _thetaI / _thetaT;
                    } else {
                        // 内部射入
                        refraction = _thetaT / _thetaI;
                        normal.y *= -1;
                    }

                    if (!math::refract(wo, normal, refraction, wi)) {
                        // 全反射
                        return 0;
                    }

                    *pdf = 1 - reflectProb;
                    Spectrum f = _albedo * (Spectrum(1.0) - reflectProb) / std::abs(wi->y);
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

            Float BXDFSpecular::samplePdf(const Vector3F &wo, const Vector3F &wi) const {
                return 0.0;
            }
        }
    }
}