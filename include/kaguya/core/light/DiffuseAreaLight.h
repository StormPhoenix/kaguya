//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_DIFFUSEAREALIGHT_H
#define KAGUYA_DIFFUSEAREALIGHT_H

#include <kaguya/core/light/AreaLight.h>

namespace kaguya {
    namespace core {

        class DiffuseAreaLight : public AreaLight {
        public:
            DiffuseAreaLight(const Spectrum &intensity,
                             std::shared_ptr<ShapeSampler> shapeSampler,
                             bool singleSide = true);

            Spectrum luminance(const Interaction &interaction, const Vector3 &wo) override;

            virtual Spectrum sampleLightRay(Ray *ray, Vector3 *normal, double *pdfPos, double *pdfDir) override;

        public:
            // 构造漫反射区域灯光
            static std::shared_ptr<AreaLight> buildDiffuseAreaLight(const Spectrum &intensity,
                                                                    std::shared_ptr<ShapeSampler> shapeSampler,
                                                                    bool singleSide = true);

        private:
            bool _singleSide = true;
        };

    }
}

#endif //KAGUYA_DIFFUSEAREALIGHT_H
