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
                                double g) :
                        _absorptionSigma(absorptionSigma),
                        _scatteringSigma(scatteringSigma),
                        _totalSigma(absorptionSigma + scatteringSigma), _g(g) {}

                virtual core::Spectrum transmittance(const tracer::Ray &ray) const override {
                    // e^{-\sigma_t * dist}
                    return exp(-_totalSigma * std::min(ray.getStep() * LENGTH(ray.getDirection()), maxDouble));
                }

                virtual core::Spectrum sampleInteraction(
                        const tracer::Ray &ray,
                        const random::Sampler1D *sampler1D,
                        MediumInteraction *mi,
                        MemoryArena &memoryArena
                ) const override {
                    // different channel has different sigma, randomly chose a channel
                    int channel = randomInt(0, SPECTRUM_CHANNEL - 1, sampler1D);

                    // sample Tr uniformly, and calculate the correspond dist
                    double dist = -std::log(1 - sampler1D->sample()) / _totalSigma[channel];
                    double step = dist / LENGTH(ray.getDirection());

                    // check whether sample the surface or medium
                    bool sampleMedium = step < ray.getStep();
                    if (sampleMedium) {
                        (*mi) = MediumInteraction(ray.at(step), ray.getDirection(), this,
                                                  ALLOC(memoryArena, HenyeyGreensteinFunction)(_g));
                    } else {
                        step = ray.getStep();
                    }

                    // calculate transmittance
                    Spectrum T = exp(-_totalSigma * std::min(step, maxDouble) * LENGTH(ray.getDirection()));

                    // calculate pdf
                    Spectrum p = sampleMedium ? _totalSigma * T : T;
                    double pdf = 0;
                    for (int i = 0; i < SPECTRUM_CHANNEL; i++) {
                        pdf += p[i];
                    }
                    pdf /= SPECTRUM_CHANNEL;
                    if (pdf == 0) {
                        assert(T.isBlack());
                    }
                    return sampleMedium ? (T * _scatteringSigma / pdf) : (T / pdf);
                }

            private:
                const Spectrum _absorptionSigma;
                const Spectrum _scatteringSigma;
                const Spectrum _totalSigma;
                double _g;
            };
        }
    }
}

#endif //KAGUYA_ISOTROPICMEDIUM_H
