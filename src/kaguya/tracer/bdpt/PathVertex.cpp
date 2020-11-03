//
// Created by Storm Phoenix on 2020/10/29.
//

#include <kaguya/core/light/AreaLight.h>
#include <kaguya/material/Material.h>
#include <kaguya/tracer/bdpt/PathVertex.h>
#include <kaguya/core/bsdf/BXDF.h>

namespace kaguya {
    namespace tracer {

        using kaguya::material::Material;
        using kaguya::core::BXDFType;

        Spectrum PathVertex::f(const PathVertex &next) {
            Vector3 worldWi = NORMALIZE(next.point - point);
            // TODO 判断类型
            switch (type) {
                case SURFACE:
                    assert(si.bsdf != nullptr);
                    return si.bsdf->f(-si.direction, worldWi);
                case CAMERA:
                case LIGHT:
                case VOLUME:
                default:
                    // TODO Not supported for now
                    assert(1 == 0);
                    return Spectrum(0.0);
            }
        }

        bool PathVertex::isDeltaLight() const {
            return (type == PathVertexType::LIGHT && ei.light != nullptr
                    && ei.light->isDeltaType());
        }

        bool PathVertex::isLight() const {
            return (type == PathVertexType::LIGHT ||
                    (type == PathVertexType::SURFACE && si.areaLight != nullptr));
        }

        Spectrum PathVertex::emit(const Vector3 &eye) const {
            if (!isLight()) {
                return Spectrum(0.0);
            }
            // 计算射向 eye 的方向
            Vector3 dirToEye = eye - point;
            if (LENGTH(dirToEye) == 0) {
                return Spectrum(0.0);
            }
            dirToEye = NORMALIZE(dirToEye);

            if (type == PathVertexType::LIGHT && ei.light != nullptr) {
                return ei.light->lightRadiance(Ray(point, dirToEye));
            } else {
                return si.areaLight->lightRadiance(si, dirToEye);
            }
        }

        const Interaction PathVertex::getInteraction() const {
            switch (type) {
                case SURFACE:
                    return si;
                case VOLUME:
                    return vi;
                default:
                    return ei;
            }
        }

        double PathVertex::computeForwardDensityPdf(double pdfWi, const PathVertex &next) const {
            double distSquare = std::pow(LENGTH(point - next.point), 2);
            double pdfFwd = pdfWi / distSquare;
            if (next.type == PathVertexType::SURFACE) {
                Vector3 surfaceNormal = next.si.normal;
                Vector3 dirToPre = -next.si.direction;
                pdfFwd *= std::abs(DOT(surfaceNormal, dirToPre));
            }
            return pdfFwd;
        }

        bool PathVertex::isConnectible() {
            // 判断类型
            switch (type) {
                case CAMERA:
                    return false;
                case LIGHT:
                    return !isDeltaLight();
                case SURFACE:
                    // 判断 specular 类型
                    assert(si.bsdf != nullptr);
                    return si.bsdf->belongToType(BXDFType(BXDFType::BSDF_DIFFUSE | BXDFType::BSDF_GLOSSY)) > 0;
                case VOLUME:
                    // TODO 暂时不支持 Volume Rendering
                    assert(1 == 0);
                    return false;
                default:
                    // TODO 暂时不支持
                    assert(1 == 0);
                    return false;
            }
        }

        double PathVertex::computeDensityPdf(const PathVertex &pre, const PathVertex &next) const {
            if (type == LIGHT) {
                return computeDensityPdfFromLight(next);
            }

            // 计算 wi
            Vector3 wi = next.point - point;
            if (LENGTH(wi) == 0) {
                return 0;
            }
            wi = NORMALIZE(wi);

            // 计算 wo
            Vector3 wo = pre.point - point;
            if (LENGTH(wo) == 0) {
                return 0;
            }
            wo = NORMALIZE(wo);

            double pdf = 0;
            // 判断类型
            // TODO 改成 if 类型
            switch (type) {
                case CAMERA:
                    // 相机类型定点，其 pdf 必定为 1
                    pdf = 1.0;
                    break;
                case SURFACE:
                    pdf = si.bsdf->samplePdf(wo, wi);
                    break;
                case VOLUME:
                    assert(1 == 0);
                    // TODO 暂时不支持
                    break;
                default:
                    assert(1 == 0);
                    // 不存在其他类型
                    break;
            }

            // pdf 转化为基于 area 的 density
            return computeForwardDensityPdf(pdf, next);
        }

        double PathVertex::computeDensityPdfFromLight(const PathVertex &next) const {
            // 获取当前 PathVertex 保存的 light 和 areaLight
            const Light *light = (type == LIGHT) ? ei.light : si.areaLight.get();
            assert(light != nullptr);

            Vector3 dirToNext = next.point - point;
            double distSquare = std::pow(LENGTH(dirToNext), 2);
            dirToNext = NORMALIZE(dirToNext);

            Ray ray = Ray(point, NORMALIZE(next.point - point));
            double pdfPos = 0;
            double pdfDir = 0;
            // 计算光源采样的 pdf
            light->randomLightRayPdf(ray, normal, &pdfPos, &pdfDir);

            // 计算 next 的 density pdf
            pdfDir /= distSquare;
            if (next.type == SURFACE) {
                pdfDir *= ABS_DOT(next.normal, dirToNext);
            }

            return pdfDir;
        }

        double PathVertex::computeDensityPdfOfLightOrigin(const PathVertex &next) const {
            Vector3 dirToNext = next.point - point;
            dirToNext = NORMALIZE(dirToNext);

            const Light *light = (type == LIGHT) ? ei.light : si.areaLight.get();
            assert(light != nullptr);

            // TODO 只考虑一个光源的情况，如果有多光源，则计算 pdf 时候要考虑到对不同光源采样的概率

            Ray ray = Ray(point, dirToNext);
            double pdfPos = 0;
            double pdfDir = 0;
            light->randomLightRayPdf(ray, normal, &pdfPos, &pdfDir);
            return pdfPos;
        }
    }
}