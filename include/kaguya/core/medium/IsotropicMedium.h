//
// Created by Storm Phoenix on 2020/12/16.
//

#ifndef KAGUYA_ISOTROPICMEDIUM_H
#define KAGUYA_ISOTROPICMEDIUM_H

#include <kaguya/core/medium/Medium.h>
#include <kaguya/core/phase/HenyeyGreensteinFunction.h>

namespace kaguya {
    namespace core {
        namespace medium {

            using kaguya::core::HenyeyGreensteinFunction;

            class IsotropicMedium : public Medium {
            public:
                IsotropicMedium(const Spectrum &absorptionSigma,
                                const Spectrum &scatteringSigma,
                                Float g) :
                        _absorptionSigma(absorptionSigma),
                        _scatteringSigma(scatteringSigma),
                        _totalSigma(absorptionSigma + scatteringSigma), _g(g) {}

                virtual core::Spectrum transmittance(const Ray &ray, Sampler *sampler) const override;

                virtual core::Spectrum sampleInteraction(
                        const tracer::Ray &ray,
                        Sampler *sampler,
                        MediumInteraction *mi,
                        MemoryArena &memoryArena
                ) const override;

            private:
                const Spectrum _absorptionSigma;
                const Spectrum _scatteringSigma;
                const Spectrum _totalSigma;
                Float _g;
            };
        }
    }
}

#endif //KAGUYA_ISOTROPICMEDIUM_H
