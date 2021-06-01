//
// Created by Storm Phoenix on 2020/12/17.
//

#include <kaguya/common.h>
#include <kaguya/core/medium/IsotropicMedium.h>

#include <algorithm>

namespace RENDER_NAMESPACE {
    namespace core {
        namespace medium {

            using namespace math;

            core::Spectrum IsotropicMedium::transmittance(const tracer::Ray &ray, Sampler *sampler) const {
                // e^{-\sigma_t * dist}
                return exp(-_sigma_t * std::min(ray.getStep() * LENGTH(ray.getDirection()), MaxFloat));
            }

            core::Spectrum
            IsotropicMedium::sampleInteraction(const tracer::Ray &ray, Sampler *sampler,
                                               MediumInteraction *mi, MemoryAllocator &allocator) const {
                // different channel has different sigma, randomly chose a channel
                int channel = math::randomInt(0, SPECTRUM_CHANNEL - 1, sampler);

                // sample1d Tr uniformly, and calculate the correspond dist
                Float dist = -std::log(1 - sampler->sample1D()) / _sigma_t[channel];
                Float step = dist / LENGTH(ray.getDirection());

                // check whether sample1d the surface or medium
                bool sampleMedium = step < ray.getStep();
                if (sampleMedium) {
                    (*mi) = MediumInteraction(ray.at(step), -ray.getDirection(), this,
                                              allocator.newObject<HenyeyGreensteinFunction>(_g));
                } else {
                    step = ray.getStep();
                }

                // calculate transmittance
                Spectrum T = exp(-_sigma_t * std::min(step, MaxFloat) * LENGTH(ray.getDirection()));

                // calculate pdf
                Spectrum p = sampleMedium ? _sigma_t * T : T;
                Float pdf = 0;
                for (int i = 0; i < SPECTRUM_CHANNEL; i++) {
                    pdf += p[i];
                }
                pdf /= SPECTRUM_CHANNEL;
                if (pdf == 0) {
                    assert(T.isBlack());
                }
                return sampleMedium ? (T * _sigma_s / pdf) : (T / pdf);
            }

        }
    }
}