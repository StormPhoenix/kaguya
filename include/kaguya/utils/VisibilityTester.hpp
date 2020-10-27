//
// Created by Storm Phoenix on 2020/10/23.
//

#ifndef KAGUYA_VISIBILITYTESTER_HPP
#define KAGUYA_VISIBILITYTESTER_HPP

#include <kaguya/core/Core.h>
#include <kaguya/core/Interaction.h>
#include <kaguya/scene/Scene.h>
// TODO Ray 放到 core
#include <kaguya/tracer/Ray.h>

namespace kaguya {
    namespace utils {

        using kaguya::core::Interaction;
        using kaguya::core::SurfaceInteraction;
        using kaguya::scene::Scene;
        using kaguya::tracer::Ray;

        class VisibilityTester {
        public:
            VisibilityTester() {}

            VisibilityTester(const Interaction start, const Interaction end) :
                    _start(start), _end(end) {
                _step = LENGTH(_end.point - _start.point);
            }

            bool isVisible(Scene &scene) {
                Ray ray(_start.point, NORMALIZE(_end.point - _start.point));
                // TODO 如何融合 Surface 和 Voluem
                SurfaceInteraction interaction;
                bool isIntersect = scene.hit(ray, interaction);

//                return (!isIntersect) || (isIntersect && interaction.step >= (_step - EPSILON));
                // TODO
                if (!isIntersect) {
                    // 如果没有击中任何物体，则 Ray 必定穿过 _end
                    return true;
                } else if (interaction.step >= (_step - EPSILON)) {
                    return true;
                } else {
                    return false;
                }
            }

        private:
            bool pointMatch(const Vector3 &a, const Vector3 &b) {
                return std::abs(a.x - b.x) < EPSILON &&
                       std::abs(a.y - b.y) < EPSILON &&
                       std::abs(a.z - b.z) < EPSILON;
            }

        private:
            Interaction _start;
            Interaction _end;
            double _step;
        };

    }
}

#endif //KAGUYA_VISIBILITYTESTER_HPP
