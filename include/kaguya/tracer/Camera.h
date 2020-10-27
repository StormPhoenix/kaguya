//
// Created by Storm Phoenix on 2020/9/29.
//

#ifndef KAGUYA_CAMERA_H
#define KAGUYA_CAMERA_H

#include <kaguya/math/Math.hpp>
#include <kaguya/tracer/Ray.h>

namespace kaguya {
    namespace tracer {

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
                   float fov = 60.0, float aspect = 1.0);

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
            Camera(const Vector3 &eye, float yaw = -90.0f, float pitch = 0.0f, float fov = 60.0, float aspect = 1.0);

            /**
             * 获取射线
             * @param u
             * @param v
             * @return
             */
            Ray sendRay(double u, double v);

            Vector3 getEye() const;

            /**
             * 获得相机成像宽度
             * @return
             */
            int getResolutionWidth();

            void setResolutionWidth(int resolutionWidth);

            /**
            * 获得相机成像高度
            * @return
            */
            int getResolutionHeight();

            void setResolutionHeight(int resolutionHeight);

        private:
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
            // 相机成像平面大小
            double _halfWindowHeight;
            double _halfWindowWidth;
        };

    }
}

#endif //KAGUYA_CAMERA_H
