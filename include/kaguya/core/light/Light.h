//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_LIGHT_H
#define KAGUYA_LIGHT_H

#include <kaguya/common.h>
#include <kaguya/core/core.h>
#include <kaguya/tracer/Ray.h>
#include <kaguya/core/Interaction.h>
#include <kaguya/utils/VisibilityTester.h>

namespace RENDER_NAMESPACE {
    namespace core {

        namespace medium {
            class MediumInterface;
        }

        using scene::Scene;
        using medium::Medium;
        using kaguya::utils::VisibilityTester;
        using kaguya::tracer::Ray;

        // 灯光类型
        typedef enum LightType {
            DELTA_POSITION = 1 << 0,
            DELTA_DIRECTION = 1 << 1,
            AREA = 1 << 2,
            ENVIRONMENT = 1 << 3
        } LightType;

        class Light {
        public:
            typedef std::shared_ptr<Light> Ptr;

            Light(LightType type, const MediumInterface &mediumInterface);

            /**
             * 计算对交点 eye 处对辐射量
             * @param eye
             * @param wi
             * @param pdf
             * @param visibilityTester 用于判断 ray 是否击中光源
             * @return
             */
            virtual Spectrum sampleLi(const Interaction &eye, Vector3F *wi, Float *pdf,
                                      Sampler *sampler,
                                      VisibilityTester *visibilityTester) = 0;

            /**
             * 计算 eye 朝 dir 方向射向 Light 的 pdf
             * @param point
             * @param dir
             * @return
             */
            virtual Float pdfLi(const Interaction &eye, const Vector3F &dir) = 0;

            /**
             * light 主动发射光线
             * @param ray 发射射线
             * @param pdfPos 射线位置 pdf
             * @param pdfDir 射线方向 pdf
             * @return
             */
            virtual Spectrum sampleLe(Ray *ray, Normal3F *normal, Float *pdfPos,
                                      Float *pdfDir, Sampler *sampler) = 0;

            /**
             * light 主动发射光线，并计算其 pdf 函数
             * （计算的 pdf 是光源发射光线的立体角的 pdf）
             * @param ray 光源发射射线
             * @param normal 光源发射射线位置处的法线向量
             * @param pdfPos
             * @param pdfDir
             * @return
             */
            virtual void pdfLe(const Ray &ray, const Vector3F &normal,
                               Float *pdfPos, Float *pdfDir) const = 0;

            /**
             * 计算从 ray->origin 起点开始，ray->direction 朝向的 Radiance
             * @param ray
             * @return
             */
            virtual Spectrum Le(const Ray &ray) const;

            virtual void worldBound(const std::shared_ptr<Scene> scene);

            LightType getType() const;

            /**
             * 判断光源是否是尖端分布
             * @return
             */
            bool isDeltaType() const;

        protected:
            const LightType _type;
            MediumInterface _mediumInterface;
        };

    }
}

#endif //KAGUYA_LIGHT_H
