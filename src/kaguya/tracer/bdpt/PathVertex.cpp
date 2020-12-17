//
// Created by Storm Phoenix on 2020/10/29.
//

#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/core/light/AreaLight.h>
#include <kaguya/material/Material.h>
#include <kaguya/tracer/bdpt/PathVertex.h>
#include <kaguya/tracer/Camera.h>

namespace kaguya {
    namespace tracer {

        using kaguya::material::Material;
        using kaguya::core::BXDFType;

        Spectrum PathVertex::f(const PathVertex &next) const {
            Vector3 worldWi = NORMALIZE(next.point - point);
            switch (type) {
                case SURFACE:
                    assert(si.getBSDF() != nullptr);
                    return si.getBSDF()->f(-si.getDirection(), worldWi);
                case MEDIUM:
                    assert(mi.getPhaseFunction() != nullptr);
                    return mi.getPhaseFunction()->scatterPdf(-mi.getDirection(), worldWi);
                case CAMERA:
                case LIGHT:
                default:
                    return Spectrum(0.0);
            }
        }

        bool PathVertex::isDeltaLight() const {
            return (type == PathVertexType::LIGHT && ei.light != nullptr
                    && ei.light->isDeltaType());
        }

        bool PathVertex::isLight() const {
            return (type == PathVertexType::LIGHT ||
                    (type == PathVertexType::SURFACE && si.getAreaLight() != nullptr));
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
                return si.getAreaLight()->lightRadiance(si, dirToEye);
            }
        }

        const Interaction PathVertex::getInteraction() const {
            switch (type) {
                case SURFACE:
                    return si;
                case MEDIUM:
                    return mi;
                case CAMERA:
                    return ei;
                case LIGHT:
                    return ei;
                default:
                    assert(1 == 0);
                    // TODO not support for now
                    return ei;
            }
        }

        double PathVertex::computeForwardDensityPdf(double pdfWi, const PathVertex &next) const {
            double distSquare = std::pow(LENGTH(point - next.point), 2);
            if (distSquare == 0) {
                return 0;
            }

            double pdfFwd = pdfWi / distSquare;
            if (next.type == PathVertexType::SURFACE) {
                Vector3 surfaceNormal = next.si.getNormal();
                Vector3 dirToPre = -next.si.getDirection();
                pdfFwd *= ABS_DOT(surfaceNormal, dirToPre);
            }
            return pdfFwd;
        }

        bool PathVertex::isConnectible() const {
            // 判断类型
            switch (type) {
                case CAMERA:
                    return true;
                case LIGHT:
                    return !isDeltaLight();
                case SURFACE:
                    // 判断 specular 类型
                    assert(si.getBSDF() != nullptr);
                    return si.getBSDF()->hasAnyOf(BXDFType(BXDFType::BSDF_DIFFUSE | BXDFType::BSDF_GLOSSY)) > 0;
                case MEDIUM:
                    return true;
                default:
                    // TODO 暂时不支持
                    assert(1 == 0);
                    return false;
            }
        }

        double PathVertex::computeDensityPdf(const PathVertex *pre, const PathVertex &next) const {
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
            Vector3 wo;
            if (pre != nullptr) {
                wo = pre->point - point;
                if (LENGTH(wo) == 0) {
                    return 0;
                }
                wo = NORMALIZE(wo);
            } else {
                // vertex type should be CAMERA
                assert(type == CAMERA);
            }

            // 计算从当前点射向 next 点的 pdf
            double pdf = 0;
            if (type == CAMERA) {
                double pdfPos = 0;
                Ray cameraRay = Ray(ei.getPoint(), ei.getDirection());
                ei.camera->rayImportance(cameraRay, pdfPos, pdf);
            } else if (type == SURFACE) {
                pdf = si.getBSDF()->samplePdf(wo, wi);
            } else if (type == MEDIUM) {
                assert(mi.getPhaseFunction() != nullptr);
                pdf = mi.getPhaseFunction()->scatterPdf(wo, wi);
            } else {
                // TODO 暂时不支持
                assert(1 == 0);
            }

            // pdf 转化为基于 area 的 density
            return computeForwardDensityPdf(pdf, next);
        }

        double PathVertex::computeDensityPdfFromLight(const PathVertex &next) const {
            // 获取当前 PathVertex 保存的 light 和 areaLight
            const Light *light = (type == LIGHT) ? ei.light : si.getAreaLight();
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

            const Light *light = (type == LIGHT) ? ei.light : si.getAreaLight();
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