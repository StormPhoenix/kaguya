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

            Spectrum lightRadiance(const Interaction &interaction, const Vector3 &wo) const override;

            virtual Spectrum randomLightRay(Ray *ray, Vector3 *normal, double *pdfPos, double *pdfDir,
                                            random::Sampler1D *sampler1D) override;

            virtual void randomLightRayPdf(const Ray &ray, const Vector3 &normal,
                                           double *pdfPos, double *pdfDir) const override;

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
