//
// Created by Storm Phoenix on 2020/10/15.
//

#ifndef KAGUYA_BXDFSPECULARREFLECTION_H
#define KAGUYA_BXDFSPECULARREFLECTION_H

#include <kaguya/core/bsdf/BXDF.h>
#include <kaguya/core/bsdf/Fresnel.h>

namespace kaguya {
    namespace core {

        /**
         * 高光类型，用于反射
         */
        class BXDFSpecularReflection : public BXDF {
        public:
            BXDFSpecularReflection(const Spectrum &albedo, Fresnel *fresnel);

            virtual Spectrum f(const Vector3F &wo, const Vector3F &wi) const override;

            virtual Spectrum sampleF(const Vector3F &wo, Vector3F *wi, Float *pdf,
                                     Sampler *const sampler1D) override;

            virtual Float samplePdf(const Vector3F &wo, const Vector3F &wi) const override;

        private:
            const Spectrum _albedo;
            Fresnel *_fresnel = nullptr;

        };

    }
}

#endif //KAGUYA_BXDFSPECULARREFLECTION_H
