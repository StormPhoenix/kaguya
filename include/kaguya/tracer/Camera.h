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

namespace kaguya {
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

            /**
             * 获取射线
             * @param u
             * @param v
             * @return
             */
            Ray sendRay(Float u, Float v) const;

            const Vector3F &getFront() const {
                return _front;
            }

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
             * 构建成像平面
             * @param channel
             * @return
             */
            FilmPlane *buildFilmPlane(int channel) const;

            /**
             * 计算相机发射射线 ray 的 importance（pdfPos，pdfDir）
             * @param ray
             * @param pdfPos
             * @param pdfDir
             * @return
             */
            void rayImportance(const Ray &ray, Float &pdfPos, Float &pdfDir) const;

        private:
            /**
             * 计算相机发射射线 ray 的 importance，并计算 filmPosition。
             * Importance 是 pdf of position 和 pdf of direction 的乘积
             * @param ray
             * @param filmPosition
             * @return
             */
            Spectrum rayImportance(const Ray &ray, Point2F *const filmPosition) const;
        private:
            // 相机位置
            Vector3F _front;
            Vector3F _right;
            // 相机成像平面面积
            Float _area;
            // 相机光圈大小
            Float _lensRadius = 0.025;
            // 默认焦距为 10
            Float _focal = 10;

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
