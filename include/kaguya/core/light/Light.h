//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_LIGHT_H
#define KAGUYA_LIGHT_H

#include <kaguya/core/Core.h>
#include <kaguya/core/Interaction.h>
#include <kaguya/tracer/Ray.h>
#include <kaguya/utils/VisibilityTester.hpp>

namespace kaguya {
    namespace core {

        using kaguya::utils::VisibilityTester;
        using kaguya::tracer::Ray;

        // 灯光类型
        typedef enum LightType {
            DELTA_POSITION = 1 << 0,
            DELTA_DIRECTION = 1 << 1,
            AREA = 1 << 2
        } LightType;

        class Light {
        public:
            Light(LightType type) : _type(type) {}

            /**
             * 计算对交点 eye 处对辐射量
             * @param eye
             * @param wi
             * @param pdf
             * @param visibilityTester 用于判断 ray 是否击中光源
             * @return
             */
            virtual Spectrum sampleFromLight(const Interaction &eye, Vector3 *wi, double *pdf,
                                             VisibilityTester *visibilityTester) = 0;

            /**
             * 计算 eye 朝 dir 方向射向 Light 的 pdf
             * @param point
             * @param dir
             * @return
             */
            virtual double sampleFromLightPdf(const Interaction &eye, const Vector3 &dir) = 0;

            /**
             * light 主动发射光线
             * @param ray 发射射线
             * @param pdfPos 射线位置 pdf
             * @param pdfDir 射线方向 pdf
             * @return
             */
            virtual Spectrum randomLightRay(Ray *ray, Vector3 *normal, double *pdfPos, double *pdfDir) = 0;

            /**
             * light 主动发射光线，并计算其 pdf 函数
             * （计算的 pdf 是光源发射光线的立体角的 pdf）
             * @param ray 光源发射射线
             * @param normal 光源发射射线位置处的法线向量
             * @param pdfPos
             * @param pdfDir
             * @return
             */
            virtual void randomLightRayPdf(const Ray &ray, const Vector3 &normal,
                                           double *pdfPos, double *pdfDir) const = 0;

            /**
             * 计算从 ray->origin 起点开始，ray->direction 朝向的 Radiance
             * @param ray
             * @return
             */
            virtual Spectrum lightRadiance(const Ray &ray) const { return Spectrum(0.0); }

            /**
             * 判断光源是否是尖端分布
             * @return
             */
            bool isDeltaType() const {
                return (_type & (DELTA_DIRECTION | DELTA_POSITION)) > 0;
            }

        protected:
            const LightType _type;
        };

    }
}

#endif //KAGUYA_LIGHT_H