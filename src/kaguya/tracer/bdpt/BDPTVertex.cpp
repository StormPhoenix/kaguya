//
// Created by Storm Phoenix on 2020/10/29.
//

#include <kaguya/scene/accumulation/AABB.h>
#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/core/bsdf/BSDF.h>
#include <kaguya/core/light/Light.h>
#include <kaguya/core/light/EnvironmentLight.h>
#include <kaguya/core/light/AreaLight.h>
#include <kaguya/tracer/bdpt/BDPTVertex.h>
#include <kaguya/tracer/Camera.h>

namespace RENDER_NAMESPACE {
    namespace tracer {

        using core::LightType;
        using core::bsdf::BSDF;
        using core::bsdf::BXDFType;
        using scene::acc::AABB;
        using material::Material;

        Float environmentLightDensity(const std::shared_ptr<Scene> scene, Vector3F toLightDirection) {
            Float density = 0.0;
            Float nLight = scene->getEnvironmentLights().size();
            Float lightPdf = 1.0 / nLight;
            for (Light::Ptr light : scene->getEnvironmentLights()) {
                density += light->pdfLi(Interaction(), toLightDirection);
            }
//            return density / scene->getEnvironmentLights().size();
            return density * lightPdf;
        }

        Spectrum BDPTVertex::f(const BDPTVertex &next) const {
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

        bool BDPTVertex::isDeltaLight() const {
            // TODO 缺少 env light 的考虑
            return (type == BDPTVertexType::LIGHT && ei.light != nullptr
                    && ei.light->isDeltaType());
        }

        bool BDPTVertex::isSurfaceType() const {
            return type == BDPTVertexType::SURFACE;
        }

        bool BDPTVertex::isLight() const {
            return (type == BDPTVertexType::LIGHT ||
                    (type == BDPTVertexType::SURFACE && si.getAreaLight() != nullptr));
        }

        bool BDPTVertex::isInfiniteLight() const {
            // ei.light == nullptr -> environment light
            // ei.light != nullptr && ei.light is env light
            // ei.light != nullptr && ei.light is deltaDirection -> sun light
            return type == BDPTVertexType::LIGHT &&
                   (ei.light == nullptr ||
                    ((ei.light->getType() & core::ENVIRONMENT) > 0) ||
                    ((ei.light->getType() & core::DELTA_DIRECTION) > 0));
        }

        Spectrum BDPTVertex::Le(std::shared_ptr<Scene> scene, const Vector3F &eye) const {
            if (!isLight()) {
                return Spectrum(0.0);
            }

            // 计算射向 eye 的方向
            Vector3F toEyeDirection = eye - point;
            if (LENGTH(toEyeDirection) == 0) {
                return Spectrum(0.0);
            }
            toEyeDirection = NORMALIZE(toEyeDirection);

            // TODO PBRT 实现的可能有点问题，没有考虑到 SunLight 的情况
            if (isInfiniteLight()) {
                Spectrum Le(0.0);
                for (Light::Ptr light: scene->getEnvironmentLights()) {
                    Le += light->Le(Ray(point, -toEyeDirection));
                }
                return Le;
            } else {
                return si.getAreaLight()->L(si, toEyeDirection);
            }
        }

        const Interaction BDPTVertex::getInteraction() const {
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

        Float BDPTVertex::convertToDensity(Float pdfWi, const BDPTVertex &next) const {
            if (next.isInfiniteLight()) {
                return pdfWi;
            }
            Vector3F wi = next.point - point;
            Float distSquare = std::pow(LENGTH(wi), 2);
            if (distSquare == 0) {
                return 0;
            }
            wi = NORMALIZE(wi);

            Float pdfFwd = pdfWi / distSquare;
            if (next.isSurfaceType()) {
                pdfFwd *= ABS_DOT(next.geometryNormal(), wi);
            }
            return pdfFwd;
        }

        bool BDPTVertex::isConnectible() const {
            // 判断类型
            switch (type) {
                case CAMERA:
                    return true;
                case LIGHT:
                    return (ei.light->getType() & (int) LightType::DELTA_DIRECTION) == 0;
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

        Float BDPTVertex::computeDensity(const Scene::Ptr scene, const BDPTVertex *pre, const BDPTVertex &next) const {
            if (type == LIGHT) {
                return densityFromLight(scene, next);
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

        Float BDPTVertex::densityFromLight(const Scene::Ptr scene, const BDPTVertex &next) const {
            Vector3F toNextDirection = next.point - point;
            Float distSquare = std::pow(LENGTH(toNextDirection), 2);
            if (distSquare == 0.) {
                return 1.0;
            }
            toNextDirection = NORMALIZE(toNextDirection);
            Float density;
            if (isInfiniteLight()) {
                const AABB &bound = scene->getWorldBox();
                Float worldRadius = 0.5 * LENGTH(bound.maxPos() - bound.minPos());
                density = 1.0 / (math::PI * worldRadius * worldRadius);
            } else {
                const Light *light = (type == LIGHT) ? ei.light : si.getAreaLight();
                ASSERT(light != nullptr, "Light  can't be nullptr. ");
                Ray ray = Ray(point, toNextDirection);
                Float pdfPos = 0;
                Float pdfDir = 0;
                light->pdfLe(ray, geometryNormal(), &pdfPos, &pdfDir);
                density = pdfDir / distSquare;
            }

            // 计算 next 的 density pdf
            if (next.type == SURFACE) {
                density *= ABS_DOT(next.geometryNormal(), toNextDirection);
            }

            return density;
        }

        Float BDPTVertex::densityLightOrigin(const Scene::Ptr scene, const BDPTVertex &next) const {
            Vector3F dirToNext = next.point - point;
            if (LENGTH(dirToNext) == 0.0) {
                return 0.0;
            }
            dirToNext = NORMALIZE(dirToNext);

            if (isInfiniteLight()) {
                return environmentLightDensity(scene, -dirToNext);
            } else {
                const Light *light = (type == LIGHT) ? ei.light : si.getAreaLight();
                ASSERT(light != nullptr, "Light can't be nullptr .");
                Ray ray = Ray(point, dirToNext);
                Float pdfPos = 0;
                Float pdfDir = 0;
                light->pdfLe(ray, geometryNormal(), &pdfPos, &pdfDir);
                Float nLight = scene->getLights().size();
                return pdfPos / nLight;
            }
        }
    }
}