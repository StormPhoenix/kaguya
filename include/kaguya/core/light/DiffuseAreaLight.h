//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_DIFFUSEAREALIGHT_H
#define KAGUYA_DIFFUSEAREALIGHT_H

#include <kaguya/core/light/AreaLight.h>
#include <kaguya/scene/Geometry.h>

namespace kaguya {
    namespace core {

        using kaguya::scene::Geometry;

        class DiffuseAreaLight : public AreaLight {
        public:
            DiffuseAreaLight(const Spectrum &intensity,
                             std::shared_ptr<Geometry> shape,
                             const MediumBound &mediumBoundary,
                             bool singleSide = true);

            Spectrum lightRadiance(const Interaction &interaction, const Vector3F &wo) const override;

            virtual Spectrum randomLightRay(Ray *ray, Vector3F *normal, Float *pdfPos, Float *pdfDir,
                                            Sampler *sampler1D) override;

            virtual void randomLightRayPdf(const Ray &ray, const Vector3F &normal,
                                           Float *pdfPos, Float *pdfDir) const override;

        public:
            // 构造漫反射区域灯光
            static std::shared_ptr<AreaLight> buildDiffuseAreaLight(const Spectrum &intensity,
                                                                    std::shared_ptr<Geometry> geometry,
                                                                    const MediumBound &mediumBoundary,
                                                                    bool singleSide = true);

        private:
            bool _singleSide = true;
        };

    }
}

#endif //KAGUYA_DIFFUSEAREALIGHT_H
