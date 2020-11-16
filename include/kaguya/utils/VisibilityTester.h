//
// Created by Storm Phoenix on 2020/10/23.
//

#ifndef KAGUYA_VISIBILITYTESTER_H
#define KAGUYA_VISIBILITYTESTER_H

#include <kaguya/core/Core.h>
#include <kaguya/core/Interaction.h>
// TODO Ray 放到 core
#include <kaguya/tracer/Ray.h>

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

            bool isVisible(Scene &scene);

            Interaction getStart() {
                return _start;
            }

            Interaction getEnd() {
                return _end;
            }

        private:
            bool pointMatch(const Vector3 &a, const Vector3 &b);

        private:
            Interaction _start;
            Interaction _end;
            double _step;
        };

    }
}

#endif //KAGUYA_VISIBILITYTESTER_H