//
// Created by Storm Phoenix on 2020/10/16.
//

#include <kaguya/core/bsdf/BXDFSpecular.h>
#include <kaguya/core/bsdf/Fresnel.h>

namespace kaguya {
    namespace core {

        BXDFSpecular::BXDFSpecular(const Spectrum &albedo, double thetaI, double thetaT) :
                BXDF(BXDFType(BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION)),
                _albedo(albedo), _thetaI(thetaI), _thetaT(thetaT) {}

        Spectrum BXDFSpecular::f(const Vector3 &wo, const Vector3 &wi) const {
            return Spectrum(0.0);
        }

        Spectrum BXDFSpecular::sampleF(const Vector3 &wo, Vector3 *wi, double *pdf) {
            double cosine = wo.y;
            // 计算反射概率
//            double reflectProb = fresnelDielectric(cosine, _thetaI, _thetaT);
            double reflectProb = schlick(cosine, _thetaI / _thetaT);

            // 随机采样是否反射
            double random = uniformSample();
            if (random < reflectProb) {
                // 反射
                *wi = Vector3(-wo.x, wo.y, -wo.z);
                *pdf = reflectProb;
                // f(p, w_o, w_i) / cos(theta(w_i))
                return reflectProb * _albedo / abs(wi->y);
            } else {
                // 折射
                double refraction;
                // 法向
                Vector3 normal = Vector3(0.0, 1.0, 0.0);
                // 判断射入方向
                if (wo.y > 0) {
                    // 外部射入
                    refraction = _thetaI / _thetaT;
                } else {
                    // 内部射入
                    refraction = _thetaT / _thetaI;
                    normal.y *= -1;
                }

                if (!refract(wo, normal, refraction, wi)) {
                    // 全反射
                    return 0;
                }

                *pdf = 1 - reflectProb;
                return _albedo * (Spectrum(1.0) - reflectProb) / std::abs(wi->y);
            }
        }

        double BXDFSpecular::samplePdf(const Vector3 &wo, const Vector3 &wi) const {
            return 0.0;
        }
    }
}