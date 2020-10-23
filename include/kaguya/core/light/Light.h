//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_LIGHT_H
#define KAGUYA_LIGHT_H

#include <kaguya/core/Core.h>
#include <kaguya/core/Interaction.h>
#include <kaguya/utils/VisibilityTester.hpp>

namespace kaguya {
    namespace core {

        using kaguya::utils::VisibilityTester;

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
            virtual Spectrum sampleRay(
                    const Interaction &eye,
                    Vector3 *wi, double *pdf,
                    VisibilityTester *visibilityTester) = 0;

            /**
             * 计算 eye 朝 dir 方向射向 Light 的 pdf
             * @param point
             * @param dir
             * @return
             */
            virtual double sampleRayPdf(const Interaction &eye, const Vector3 &dir) = 0;

            bool isDeltaType() {
                return (_type & (DELTA_DIRECTION | DELTA_POSITION)) > 0;
            }

        protected:
            const LightType _type;
        };

    }
}

#endif //KAGUYA_LIGHT_H
