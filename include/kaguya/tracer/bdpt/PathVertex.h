//
// Created by Storm Phoenix on 2020/10/29.
//

#ifndef KAGUYA_PATHVERTEX_H
#define KAGUYA_PATHVERTEX_H

#include <kaguya/core/Interaction.h>
#include <kaguya/core/light/Light.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace kaguya {
    namespace tracer {

        using kaguya::core::Light;
        using kaguya::core::Spectrum;

        using kaguya::core::Interaction;
        using kaguya::core::StartEndInteraction;
        using kaguya::core::SurfaceInteraction;
        using kaguya::core::MediumInteraction;

        typedef enum PathVertexType {
            CAMERA, LIGHT, SURFACE, MEDIUM
        } PathVertexType;

        typedef struct PathVertex {
            // f(p, wo, wi) * cosine / p(w_i)
            Spectrum beta;
            // 路径点类型
            PathVertexType type;
            // 路径点位置
            Vector3F point;
            // 当前点的概率密度，从上一个点发射射线选取
            Float pdfForward = 0;
            // 当前点的概率密度，从下一个点发射射线选取
            Float pdfBackward = 0;
            // 当前点的前后传输射线是否是 delta 分布
            bool isDelta = false;
            // 交互点
            SurfaceInteraction si;
            MediumInteraction mi;
            StartEndInteraction ei;

            PathVertex() {}

            PathVertex(const SurfaceInteraction &si, const Spectrum &beta) :
                    si(si), beta(beta), point(si.point), type(PathVertexType::SURFACE) {}

            PathVertex(const MediumInteraction &mi, const Spectrum &beta) :
                    mi(mi), beta(beta), point(mi.point), type(PathVertexType::MEDIUM) {}

            PathVertex(PathVertexType type, const StartEndInteraction &ei, const Spectrum &beta) :
                    type(type), ei(ei), point(ei.point), beta(beta) {
            }

            Normal3F geometryNormal() const {
                return getInteraction().normal;
            }

            Normal3F shadingNormal() const {
                if (type == PathVertexType::SURFACE) {
                    return getInteraction().rendering.normal;
                } else {
                    return getInteraction().normal;
                }
            }

            /**
             * 是否可以与其他点做连接
             *
             * @return
             */
            bool isConnectible() const;

            /**
             * 计算 PathVertex 位置处到下一个点 next 处的 bsdf 值
             * @param p
             * @return
             */
            Spectrum f(const PathVertex &next) const;

            /**
             * 当前点发出射线指向下一个点 next，计算 next 的概率密度
             * (将 next 点基于 angle 的密度转化成基于 area 的密度)
             * @param pdfWi 当前点向 wi 方向发射射线的概率
             * @param next 下一个点
             * @return
             */
            Float convertDensity(Float pdfWi, const PathVertex &next) const;

            /**
             * 计算 pre -> p -> next 的 density pdf，这个 ，density pdf 是 next 的。
             * 计算方法和 computePdfForward 一致，
             * 不同之处在于 computePdfForward 是在已知 pdfWi 的情况下计算的，
             * computePdf 需要自己计算 pdfWi
             * @param pre
             * @param next
             * @return
             */
            Float density(const PathVertex *pre, const PathVertex &next) const;

            /**
             * 计算 p -> next 的 density pdf（密度 pdf），这个 density pdf 是 next 的 density pdf，
             * 与 computePdf 类似，
             * (区别在于 p 是 Light 类型)
             * @param next
             * @return
             */
            Float densityByLight(const PathVertex &next) const;

            /**
             * 当前 PathVertex 作为发光体的时候，计算这个发光体上的发光点的 density pdf
             * @param next
             * @return
             */
            Float densityLightOrigin(const PathVertex &next) const;

            const Interaction getInteraction() const;

            /**
             * 判断 PathVertex 是否是光源并且为 Delta 分布
             * @return
             */
            bool isDeltaLight() const;

            /**
             * 判断是否是光源类型
             * @return
             */
            bool isLight() const;

            /**
             * 假设 PathVertex 可以发光，则此处计算 PathVertex 向 eye 方向投射射线的 Radiance
             *
             * @param eye
             * @return
             */
            Spectrum emit(const Vector3F &eye) const;

            static inline PathVertex createCameraVertex(const Camera *camera, const Ray &ray, Spectrum beta) {
                StartEndInteraction ei = StartEndInteraction(camera, ray);
                PathVertex cameraVertex = PathVertex(PathVertexType::CAMERA, ei, beta);
                cameraVertex.pdfForward = 1.0;
                return cameraVertex;
            }

            static inline PathVertex createMediumVertex(const MediumInteraction &mi, Float forwardDensity,
                                                        const PathVertex &preVertex, const Spectrum &beta) {
                PathVertex vertex = PathVertex(mi, beta);
                vertex.pdfForward = preVertex.convertDensity(forwardDensity, vertex);
                return vertex;
            }

            static inline PathVertex createLightVertex(const Light *light, const Vector3F &p, const Vector3F &dir,
                                                       const Vector3F &n, const Spectrum &intensity, Float pdf) {
                StartEndInteraction ei = StartEndInteraction(light, p, dir, n);
                PathVertex pathVertex = PathVertex(PathVertexType::LIGHT, ei, intensity);
                pathVertex.pdfForward = pdf;
                return pathVertex;
            }


            static inline PathVertex createLightVertex(const StartEndInteraction &ei, Spectrum &beta) {
                return PathVertex(PathVertexType::LIGHT, ei, beta);
            }

            static inline PathVertex createSurfaceVertex(const SurfaceInteraction &si,
                                                         Float pdfPreWi,
                                                         const PathVertex &pre,
                                                         const Spectrum &beta) {
                // 创建路径点
                PathVertex v = PathVertex(si, beta);
                // 计算上个点发射线击中当前点时，当前点对应的概率
                v.pdfForward = pre.convertDensity(pdfPreWi, v);
                return v;
            }

        } PathVertex;


    }
}

#endif //KAGUYA_PATHVERTEX_H
