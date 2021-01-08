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

            virtual Spectrum f(const Vector3d &wo, const Vector3d &wi) const override;

            virtual Spectrum sampleF(const Vector3d &wo, Vector3d *wi, double *pdf,
                                     Sampler *const sampler1D) override;

            virtual double samplePdf(const Vector3d &wo, const Vector3d &wi) const override;

        private:
            const Spectrum _albedo;
            Fresnel *_fresnel = nullptr;

        };

    }
}

#endif //KAGUYA_BXDFSPECULARREFLECTION_H
