//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_CAMERA_H
#define KAGUYA_CAMERA_H

#include <kaguya/core/Interaction.h>
#include <kaguya/math/Math.h>
#include <kaguya/tracer/Ray.h>
#include <kaguya/tracer/FilmPlane.h>
#include <kaguya/utils/VisibilityTester.h>
#include <kaguya/core/Transform.h>
#include <kaguya/core/medium/Medium.h>

namespace RENDER_NAMESPACE {
    namespace tracer {

        using kaguya::core::Interaction;
        using kaguya::utils::VisibilityTester;
        using kaguya::core::medium::Medium;
        using kaguya::core::transform::Transform;
        using kaguya::sampler::Sampler;

        class Camera {
        public:
            typedef std::shared_ptr<Camera> Ptr;

            Camera(Transform::Ptr cameraToWorld, Float hFov, Float nearClip = 1.0, Float farClip = 10000,
                   Medium::Ptr medium = nullptr);

            Ray generateRay(Float pixelX, Float pixelY, Sampler *sampler) const;

            /**
             * 相机对 eye 发射采样射线，起点随机
             * @param eye
             * @param wi
             * @param pdf
             * @param filmPosition
             * @param visibilityTester
             * @return
             */
            Spectrum sampleCameraRay(const Interaction &eye, Vector3F *wi, Float *pdf, Point2F *filmPosition,
                                     Sampler *const sampler, VisibilityTester *visibilityTester) const;

            /**
           * 计算相机发射射线 ray 的 importance（pdfPos，pdfDir）
           * @param ray
           * @param pdfPos
           * @param pdfDir
           * @return
           */
            void pdfWe(const Ray &ray, Float &pdfPos, Float &pdfDir) const;

            Point2I worldToRaster(const Point3F &point) const;

            /**
             * 构建成像平面
             * @param channel
             * @return
             */
            FilmPlane *buildFilmPlane(int channel) const;

            const Vector3F &getFront() const {
                return _front;
            }

        private:
            /**
             * 计算相机发射射线 ray 的 importance，并计算 filmPosition。
             * Importance 是 pdf of position 和 pdf of direction 的乘积
             * @param ray
             * @param filmPosition
             * @return
             */
            Spectrum We(const Ray &ray, Point2F *const filmPosition) const;

        private:
            Vector3F _origin;
            Vector3F _front;
            Float _filmPlaneArea;
            Float _lensArea;
            Float _lensRadius = 0.00025;

            Float _nearClip;
            Float _farClip;
            // medium
            Medium::Ptr _medium;

            Transform::Ptr _cameraToWorld;
            Transform::Ptr _worldToCamera;
            Transform::Ptr _rasterToCamera;
            Transform::Ptr _cameraToRaster;
        };

    }
}

#endif //KAGUYA_CAMERA_H
