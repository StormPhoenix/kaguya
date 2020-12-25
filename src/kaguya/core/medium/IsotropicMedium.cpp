//
// Created by Storm Phoenix on 2020/12/17.
//

#include <kaguya/core/medium/IsotropicMedium.h>

namespace kaguya {
    namespace core {
        namespace medium {

            core::Spectrum IsotropicMedium::transmittance(const tracer::Ray &ray, const Sampler1D *sampler1D) const {
                // e^{-\sigma_t * dist}
                return exp(-_totalSigma * std::min(ray.getStep() * LENGTH(ray.getDirection()), math::maxDouble));
            }

            core::Spectrum
            IsotropicMedium::sampleInteraction(const tracer::Ray &ray, const Sampler1D *sampler1D,
                                               MediumInteraction *mi, MemoryArena &memoryArena) const {
                // different channel has different sigma, randomly chose a channel
                int channel = math::randomInt(0, SPECTRUM_CHANNEL - 1, sampler1D);

                // sample Tr uniformly, and calculate the correspond dist
                double dist = -std::log(1 - sampler1D->sample()) / _totalSigma[channel];
                double step = dist / LENGTH(ray.getDirection());

                // check whether sample the surface or medium
                bool sampleMedium = step < ray.getStep();
                if (sampleMedium) {
                    (*mi) = MediumInteraction(ray.at(step), -ray.getDirection(), this,
                                              ALLOC(memoryArena, HenyeyGreensteinFunction)(_g));
                } else {
                    step = ray.getStep();
                }

                // calculate transmittance
                Spectrum T = exp(-_totalSigma * std::min(step, math::maxDouble) * LENGTH(ray.getDirection()));

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

        }
    }
}