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
            Vector3F worldWi = NORMALIZE(next.point - point);
            switch (type) {
                case SURFACE:
                    ASSERT(si.bsdf != nullptr, "BSDF entity can't be null.");
                    return si.bsdf->f(-si.direction, worldWi);
                case MEDIUM:
                    ASSERT(mi.getPhaseFunction() != nullptr, "Phase function can't be null.");
                    return mi.getPhaseFunction()->scatterPdf(-mi.direction, worldWi);
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

        Spectrum PathVertex::Le(const Vector3F &eye) const {
            if (!isLight()) {
                return Spectrum(0.0);
            }
            // 计算射向 eye 的方向
            Vector3F dirToEye = eye - point;
            if (LENGTH(dirToEye) == 0) {
                return Spectrum(0.0);
            }
            dirToEye = NORMALIZE(dirToEye);

            if (type == PathVertexType::LIGHT && ei.light != nullptr) {
                return ei.light->Le(Ray(point, dirToEye));
            } else {
                return si.getAreaLight()->L(si, dirToEye);
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
                    ASSERT(false, "Interaction type not supported.");
                    // TODO not support for now
                    return ei;
            }
        }

        Float PathVertex::convertToDensity(Float pdfWi, const PathVertex &next) const {
            Vector3F wi = next.point - point;
            Float distSquare = std::pow(LENGTH(wi), 2);
            if (distSquare == 0) {
                return 0;
            }
            wi = NORMALIZE(wi);

            Float pdfFwd = pdfWi / distSquare;
            if (next.type == PathVertexType::SURFACE) {
                pdfFwd *= ABS_DOT(next.geometryNormal(), wi);
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
                    ASSERT(si.bsdf != nullptr, "BSDF can't be null.");
                    return si.bsdf->hasAnyOf(BXDFType(BXDFType::BSDF_DIFFUSE | BXDFType::BSDF_GLOSSY)) > 0;
                case MEDIUM:
                    return true;
                default:
                    // TODO 暂时不支持
                    ASSERT(false, "Type is not connectible.");
                    return false;
            }
        }

        Float PathVertex::computeDensity(const PathVertex *pre, const PathVertex &next) const {
            if (type == LIGHT) {
                return densityFromLight(next);
            }

            // 计算 wi
            Vector3F wi = next.point - point;
            if (LENGTH(wi) == 0) {
                return 0;
            }
            wi = NORMALIZE(wi);

            // 计算 wo
            Vector3F wo;
            if (pre != nullptr) {
                wo = pre->point - point;
                if (LENGTH(wo) == 0) {
                    return 0;
                }
                wo = NORMALIZE(wo);
            } else {
                // vertex type should be CAMERA
                ASSERT(type == CAMERA, "Type must be CAMERA type.");
            }

            // 计算从当前点射向 next 点的 pdf
            Float pdf = 0;
            if (type == CAMERA) {
                Float pdfPos = 0;
                Ray cameraRay = Ray(ei.point, ei.direction);
                ei.camera->pdfWe(cameraRay, pdfPos, pdf);
            } else if (type == SURFACE) {
                pdf = si.bsdf->samplePdf(wo, wi);
            } else if (type == MEDIUM) {
                ASSERT(mi.getPhaseFunction() != nullptr, "Phase function can't be null.");
                pdf = mi.getPhaseFunction()->scatterPdf(wo, wi);
            } else {
                // TODO 暂时不支持
                ASSERT(false, "Density calculation not supported.");
            }

            // pdf 转化为基于 area 的 density
            return convertToDensity(pdf, next);
        }

        Float PathVertex::densityFromLight(const PathVertex &next) const {
            // 获取当前 PathVertex 保存的 light 和 areaLight
            const Light *light = (type == LIGHT) ? ei.light : si.getAreaLight();
            ASSERT(light != nullptr, "Light entity can't be null. ");

            Vector3F dirToNext = next.point - point;
            Float distSquare = std::pow(LENGTH(dirToNext), 2);
            dirToNext = NORMALIZE(dirToNext);

            Ray ray = Ray(point, NORMALIZE(next.point - point));
            Float pdfPos = 0;
            Float pdfDir = 0;
            // 计算光源采样的 pdf
            light->pdfLe(ray, geometryNormal(), &pdfPos, &pdfDir);

            // 计算 next 的 density pdf
            pdfDir /= distSquare;
            if (next.type == SURFACE) {
                pdfDir *= ABS_DOT(next.geometryNormal(), dirToNext);
            }

            return pdfDir;
        }

        Float PathVertex::densityLightOrigin(const PathVertex &next) const {
            Vector3F dirToNext = next.point - point;
            dirToNext = NORMALIZE(dirToNext);

            const Light *light = (type == LIGHT) ? ei.light : si.getAreaLight();
            ASSERT(light != nullptr, "Light entity can't be null.");

            // TODO 只考虑一个光源的情况，如果有多光源，则计算 pdf 时候要考虑到对不同光源采样的概率

            Ray ray = Ray(point, dirToNext);
            Float pdfPos = 0;
            Float pdfDir = 0;
            light->pdfLe(ray, geometryNormal(), &pdfPos, &pdfDir);
            return pdfPos;
        }
    }
}