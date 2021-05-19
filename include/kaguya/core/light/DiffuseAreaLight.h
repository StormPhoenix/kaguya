//
// Created by Storm Phoenix on 2020/10/22.
//

#ifndef KAGUYA_DIFFUSEAREALIGHT_H
#define KAGUYA_DIFFUSEAREALIGHT_H

#include <kaguya/core/light/AreaLight.h>

namespace kaguya {
    namespace core {

        using kaguya::scene::Geometry;

        class DiffuseAreaLight : public AreaLight {
        public:
            DiffuseAreaLight(const Spectrum &intensity,
                             std::shared_ptr<Shape> shape,
                             const MediumInterface &mediumBoundary,
                             bool singleSide = true);

            Spectrum L(const Interaction &interaction, const Vector3F &wo) const override;

            virtual Spectrum sampleLe(Ray *ray, Normal3F *normal, Float *pdfPos, Float *pdfDir,
                                      Sampler *sampler) override;

            virtual void pdfLe(const Ray &ray, const Vector3F &normal,
                               Float *pdfPos, Float *pdfDir) const override;

        private:
            bool _singleSide = true;
        };

    }
}

#endif //KAGUYA_DIFFUSEAREALIGHT_H
