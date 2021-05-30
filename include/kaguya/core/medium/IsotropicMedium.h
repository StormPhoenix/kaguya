//
// Created by Storm Phoenix on 2020/12/16.
//

#ifndef KAGUYA_ISOTROPICMEDIUM_H
#define KAGUYA_ISOTROPICMEDIUM_H

#include <kaguya/core/medium/Medium.h>
#include <kaguya/core/phase/HenyeyGreensteinFunction.h>

namespace RENDER_NAMESPACE {
    namespace core {
        namespace medium {

            using core::HenyeyGreensteinFunction;

            class IsotropicMedium : public Medium {
            public:
                IsotropicMedium(const Spectrum &sigma_a,
                                const Spectrum &sigma_s,
                                Float g) :
                        _sigma_a(sigma_a),
                        _sigma_s(sigma_s),
                        _sigma_t(sigma_a + sigma_s), _g(g) {}

                virtual core::Spectrum transmittance(const Ray &ray, Sampler *sampler) const override;

                virtual core::Spectrum sampleInteraction(
                        const tracer::Ray &ray,
                        Sampler *sampler,
                        MediumInteraction *mi,
                        MemoryAllocator &allocator
                ) const override;

            private:
                const Spectrum _sigma_a;
                const Spectrum _sigma_s;
                const Spectrum _sigma_t;
                Float _g;
            };
        }
    }
}

#endif //KAGUYA_ISOTROPICMEDIUM_H
