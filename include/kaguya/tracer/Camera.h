//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_CAMERA_H
#define KAGUYA_CAMERA_H

#include <kaguya/core/Interaction.h>
#include <kaguya/math/Math.hpp>
#include <kaguya/tracer/Ray.h>
#include <kaguya/tracer/FilmPlane.h>
#include <kaguya/utils/VisibilityTester.h>

namespace kaguya {

    namespace core {
        namespace medium {
            class Medium;
        }
    }

    namespace tracer {

        using kaguya::core::Interaction;
        using kaguya::utils::VisibilityTester;
        using kaguya::core::medium::Medium;
        using kaguya::math::random::Sampler;

        class Camera {
        public:
            /**
             * 初始化相机
             * @param eye 相机位置
             * @param direction 相机朝向
             * @param fov 相机张角，角度单位
             * @param focal 相机焦距
             * @param aspect 相机成像平面的宽/高比例
             */
            Camera(const Vector3 &eye = {0.0f, 0.0f, 0.0f}, const Vector3 &direction = {0.0f, 0.0f, -1.0f},
                   const std::shared_ptr<Medium> medium = nullptr, float fov = 60.0, float aspect = 1.0);

            /**
             * 初始化相机，设计到欧拉角的计算，这和欧拉角具体的转换方式有关。
             * 此处以 x 轴指向为默认指向，先绕世界坐标 y 轴做 yaw 角度旋转（顺时针为正），
             * 再绕物体坐标系的 z 轴线做 pitch 角度旋转（逆时针为正）
             * @param eye 相机位置
             * @param fov 张角
             * @param focal 焦距
             * @param aspect 宽高比例
             * @param yaw 欧拉角，偏航角
             * @param pitch 欧拉角，俯仰角
             */
            Camera(const Vector3 &eye, float yaw = -90.0f, float pitch = 0.0f,
                   const std::shared_ptr<Medium> medium = nullptr, float fov = 60.0, float aspect = 1.0);

            /**
             * 获取射线
             * @param u
             * @param v
             * @return
             */
            Ray sendRay(double u, double v) const;

            Vector3 getEye() const;

            /**
             * 获得相机成像宽度
             * @return
             */
            int getResolutionWidth() const;

            void setResolutionWidth(int resolutionWidth);

            /**
            * 获得相机成像高度
            * @return
            */
            int getResolutionHeight() const;

            void setResolutionHeight(int resolutionHeight);

            const Vector3 &getFront() const {
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
            Spectrum sampleCameraRay(const Interaction &eye, Vector3 *wi, double *pdf, Point2d *filmPosition,
                                     const Sampler *const sampler1D,
                                     VisibilityTester *visibilityTester) const;

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
            void rayImportance(const Ray &ray, double &pdfPos, double &pdfDir) const;

        private:
            /**
             * 计算相机发射射线 ray 的 importance，并计算 filmPosition。
             * Importance 是 pdf of position 和 pdf of direction 的乘积
             * @param ray
             * @param filmPosition
             * @return
             */
            Spectrum rayImportance(const Ray &ray, Point2d *const filmPosition) const;

            /**
             * 构建相机坐标系
             * @param fov
             * @param aspect
             */
            void buildCameraCoordinate(float fov, float aspect);

        private:
            // 相机宽度分辨率
            int _resolutionWidth = 700;
            // 相机高度分辨率
            int _resolutionHeight = 700;
            // 相机位置
            Vector3 _eye;
            Vector3 _front;
            Vector3 _right;
            Vector3 _up;
            Vector3 _leftBottomCorner;
            Vector3 _rightTopCorner;
            Vector3 _diagonalVector;
            // 相机成像平面大小
            double _halfWindowHeight;
            double _halfWindowWidth;
            // 相机成像平面面积
            double _area;
            // 相机光圈大小
            double _lensRadius = 0.025;
            // 默认焦距为 10
            const double _focal = 10;
            // medium
            std::shared_ptr<Medium> _medium;
        };

    }
}

#endif //KAGUYA_CAMERA_H
