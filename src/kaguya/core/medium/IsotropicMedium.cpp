//
// Created by Storm Phoenix on 2020/12/17.
//

#include <kaguya/core/medium/IsotropicMedium.h>

namespace kaguya {
    namespace core {
        namespace medium {

            core::Spectrum IsotropicMedium::transmittance(const tracer::Ray &ray, Sampler *sampler1D) const {
                // e^{-\sigma_t * dist}
                return exp(-_totalSigma * std::min(ray.getStep() * LENGTH(ray.getDirection()), math::MAX_FLOAT));
            }

            core::Spectrum
            IsotropicMedium::sampleInteraction(const tracer::Ray &ray, Sampler *sampler1D,
                                               MediumInteraction *mi, MemoryArena &memoryArena) const {
                // different channel has different sigma, randomly chose a channel
                int channel = math::randomInt(0, SPECTRUM_CHANNEL - 1, sampler1D);

                // sample1d Tr uniformly, and calculate the correspond dist
                Float dist = -std::log(1 - sampler1D->sample1D()) / _totalSigma[channel];
                Float step = dist / LENGTH(ray.getDirection());

                // check whether sample1d the surface or medium
                bool sampleMedium = step < ray.getStep();
                if (sampleMedium) {
                    (*mi) = MediumInteraction(ray.at(step), -ray.getDirection(), this,
                                              ALLOC(memoryArena, HenyeyGreensteinFunction)(_g));
                } else {
                    step = ray.getStep();
                }

                // calculate transmittance
                Spectrum T = exp(-_totalSigma * std::min(step, math::MAX_FLOAT) * LENGTH(ray.getDirection()));

                // calculate pdf
                Spectrum p = sampleMedium ? _totalSigma * T : T;
                Float pdf = 0;
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