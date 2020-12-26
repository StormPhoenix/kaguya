//
// Created by Storm Phoenix on 2020/10/23.
//

#ifndef KAGUYA_VISIBILITYTESTER_H
#define KAGUYA_VISIBILITYTESTER_H

#include <kaguya/core/Core.h>
#include <kaguya/core/Interaction.h>
// TODO Ray 放到 core
#include <kaguya/tracer/Ray.h>
#include <kaguya/core/spectrum/Spectrum.hpp>

namespace kaguya {
    namespace scene {
        class Scene;
    }
}

namespace kaguya {
    namespace utils {

        using kaguya::core::Interaction;
        using kaguya::core::SurfaceInteraction;
        using kaguya::scene::Scene;
        using kaguya::tracer::Ray;

        class VisibilityTester {
        public:
            VisibilityTester() {}

            VisibilityTester(const Interaction start, const Interaction end);

            /**
             * T(p, p') = exponent^(-shift_{0, (p'-p)} theta_t(t) dt)
             *
             * @return
             */
            core::Spectrum transmittance(Scene &scene, const Sampler *sampler1D) const;

            bool isVisible(Scene &scene) const;

            Interaction getStart() {
                return _start;
            }

            Interaction getEnd() {
                return _end;
            }

        private:
            Interaction _start;
            Interaction _end;
        };

    }
}

#endif //KAGUYA_VISIBILITYTESTER_H
