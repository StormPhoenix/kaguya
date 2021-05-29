//
// Created by Storm Phoenix on 2020/12/12.
//

#ifndef KAGUYA_BSSRDF_H
#define KAGUYA_BSSRDF_H

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/core/Interaction.h>
#include <kaguya/scene/Scene.h>
#include <kaguya/utils/memory/MemoryAllocator.h>

namespace RENDER_NAMESPACE {
    namespace core {
        namespace bssrdf {

            using memory::MemoryAllocator;
            using scene::Scene;

            /*
             * Bidirectional subsurface scattering reflection distribution function
             * L_o(p_o, w_o) = \int_A \int_{H^2} S(p_o, p_i, w_o, w_i) L_i(p_i, w_i) cos|\theta_i| d(w_i) d(A)
             **/
            class BSSRDF {
            public:
                BSSRDF(const SurfaceInteraction &po) : po(po) {}

                /**
                 * S(p_o, p_i, w_o, w_i)
                 */
                virtual Spectrum S(const SurfaceInteraction &si, const Vector3F &wi) = 0;

                /**
                 * Sample S(p_o, p_i, w_o, w_i)
                 */
                virtual Spectrum sampleS(std::shared_ptr<Scene> scene, SurfaceInteraction *pi, Float *pdf,
                                         MemoryAllocator &allocator, Sampler *sampler) = 0;

            protected:
                const SurfaceInteraction &po;
            };
        }
    }
}

#endif //KAGUYA_SEPARABLEBSSRDF_H
