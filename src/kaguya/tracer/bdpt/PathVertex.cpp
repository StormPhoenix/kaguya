//
// Created by Storm Phoenix on 2020/10/29.
//

#include <kaguya/tracer/bdpt/PathVertex.h>
#include <kaguya/material/Material.h>

namespace kaguya {
    namespace tracer {

        using kaguya::material::Material;

        Spectrum PathVertex::f(const PathVertex &p) {
            Vector3 worldWi = NORMALIZE(p.point - point);
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

        double PathVertex::computePdfForward(const double pdfWi, const PathVertex &next) const {
            double distSquare = std::pow(LENGTH(point - next.point), 2);
            double pdfFwd = pdfWi / distSquare;
            if (next.type == PathVertexType::SURFACE) {
                Vector3 surfaceNormal = next.si.normal;
                Vector3 dirToPre = -next.si.direction;
                pdfFwd *= std::abs(DOT(surfaceNormal, dirToPre));
            }
            return pdfFwd;
        }

        PathVertex PathVertex::createCameraVertex(const Camera *camera) {
            StartEndInteraction ei = StartEndInteraction(camera);
            return PathVertex(PathVertexType::CAMERA, ei, Spectrum(1.0));
        }

        PathVertex PathVertex::createSurfaceVertex(const SurfaceInteraction &si,
                                                   double pdfPreWi,
                                                   const PathVertex &pre,
                                                   const Spectrum &beta) {
            // 创建路径点
            PathVertex v = PathVertex(si, beta);
            // 计算上个点发射线击中当前点时，当前点对应的概率
            v.pdfForward = pre.computePdfForward(pdfPreWi, v);
            return v;
        }

        PathVertex PathVertex::createLightVertex(const StartEndInteraction &ei,
                                                 kaguya::core::Spectrum &beta) {
            return PathVertex(PathVertexType::LIGHT, ei, beta);
        }

        PathVertex PathVertex::createLightVertex(const Light *light, const Vector3 &p, const Vector3 &dir,
                                                 const Vector3 &n, const Spectrum &intensity) {
            StartEndInteraction ei = StartEndInteraction(light, p, dir, n);
            PathVertex pathVertex = PathVertex(PathVertexType::LIGHT, ei, intensity);
            return pathVertex;
        }


    }
}