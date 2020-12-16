//
// Created by Storm Phoenix on 2020/11/13.
//

#include <kaguya/scene/Scene.h>
#include <kaguya/utils/VisibilityTester.h>

namespace kaguya {
    namespace utils {

        VisibilityTester::VisibilityTester(const kaguya::core::Interaction start, const kaguya::core::Interaction end) :
                _start(start), _end(end) {
            _step = LENGTH(_end.getPoint() - _start.getPoint());
        }

        bool VisibilityTester::isVisible(Scene &scene) {
            Ray ray(_start.getPoint(), NORMALIZE(_end.getPoint() - _start.getPoint()));
            // TODO 如何融合 Surface 和 Volume
            SurfaceInteraction interaction;
            bool isIntersect = scene.intersect(ray, interaction);

            return (!isIntersect) || (isIntersect && interaction.getStep() >= (_step - EPSILON));
        }

        bool VisibilityTester::pointMatch(const Vector3 &a, const Vector3 &b) {
            return std::abs(a.x - b.x) < EPSILON &&
                   std::abs(a.y - b.y) < EPSILON &&
                   std::abs(a.z - b.z) < EPSILON;
        }

    }
}