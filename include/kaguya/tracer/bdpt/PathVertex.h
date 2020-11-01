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
        using kaguya::core::VolumeInteraction;

        typedef enum PathVertexType {
            CAMERA, LIGHT, SURFACE, VOLUME
        } PathVertexType;

        typedef struct PathVertex {
            // f(p, wo, wi) * cosine / p(w_i)
            Spectrum beta;
            // 路径点类型
            PathVertexType type;
            // 路径点位置
            Vector3 point;
            // 路径点法向位置，只针对 Surface 类型有效
            Vector3 normal;
            // 当前点的概率密度，从上一个点发射射线选取
            double pdfForward = 0;
            // 当前点的概率密度，从下一个点发射射线选取
            double pdfBackward = 0;
            // 当前点的前后传输射线是否是 delta 分布
            bool isDelta = false;
            // 交互点
            struct {
                SurfaceInteraction si;
                VolumeInteraction vi;
                StartEndInteraction ei;
            };

            PathVertex() {}

            PathVertex(const SurfaceInteraction &si, const Spectrum &beta) :
                    si(si), beta(beta), point(si.point), normal(si.normal), type(PathVertexType::SURFACE) {}

            PathVertex(const VolumeInteraction &vi, const Spectrum &beta) :
                    vi(vi), beta(beta), point(vi.point), type(PathVertexType::VOLUME) {}

            PathVertex(PathVertexType type, const StartEndInteraction &ei, const Spectrum &beta) :
                    type(type), ei(ei), point(ei.point), beta(beta) {}

            // 是否可以与其他点做连接
            bool isConnectible();

            Spectrum f(const PathVertex &p);

            /**
             * 当前点发出射线指向下一个点 next，计算 next 的概率密度
             * @param pdfWi 当前点向 wi 方向发射射线的概率
             * @param next 下一个点
             * @return
             */
            double computePdfForward(double pdfWi, const PathVertex &next) const;

            const Interaction getInteraction() const;

            /**
             * 判断 PathVertex 是否是光源并且为 Delta 分布
             * @return
             */
            bool isDeltaLight() const;

            static inline PathVertex createCameraVertex(const Camera *camera);

            static inline PathVertex createLightVertex(const Light *light, const Vector3 &p, const Vector3 &dir,
                                                       const Vector3 &n, const Spectrum &intensity);

            static inline PathVertex createLightVertex(const StartEndInteraction &ei, Spectrum &beta);

            static inline PathVertex createSurfaceVertex(const SurfaceInteraction &si,
                                                         double pdfPreWi,
                                                         const PathVertex &pre,
                                                         const Spectrum &beta);

        } PathVertex;


    }
}

#endif //KAGUYA_PATHVERTEX_H
