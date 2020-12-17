//
// Created by Storm Phoenix on 2020/12/16.
//

#ifndef KAGUYA_MEDIUM_H
#define KAGUYA_MEDIUM_H

#include <kaguya/core/Core.h>
#include <kaguya/core/Interaction.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/tracer/Ray.h>
#include <kaguya/math/Sampler.hpp>
#include <kaguya/utils/MemoryArena.h>

namespace kaguya {
    namespace core {
        namespace medium {

            using kaguya::memory::MemoryArena;

            class Medium {
            public:
                // 计算 ray 的透射率
                virtual core::Spectrum transmittance(const tracer::Ray &ray) const = 0;

                // 在 medium 中采样 interaction
                virtual core::Spectrum sampleInteraction(
                        const tracer::Ray &ray,
                        const Sampler1D *sampler1D,
                        MediumInteraction *mi,
                        MemoryArena &memoryArena
                ) const = 0;
            };

        }
    }
}

#endif //KAGUYA_MEDIUM_H
