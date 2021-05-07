//
// Created by Storm Phoenix on 2021/4/15.
//

#ifndef KAGUYA_SPHERICALMAPPING2D_H
#define KAGUYA_SPHERICALMAPPING2D_H

#include <kaguya/core/Transform.h>
#include <kaguya/material/texture/TextureMapping2D.h>

namespace kaguya {
    namespace material {
        namespace texture {
            using namespace core;

            class SphericalMapping2D : public TextureMapping2D {
            public:
                SphericalMapping2D(std::shared_ptr<transform::Transform> modelViewMatrix)
                        : _modelViewMatrix(modelViewMatrix) {
                    if (_modelViewMatrix != nullptr) {
                        _worldToLocalMatrix = _modelViewMatrix->inverse().ptr();
                    } else {
                        _identity = true;
                    }
                }

                virtual Point2F map(const SurfaceInteraction &si) override {
                    Point3F dir;
                    if (!_identity) {
                        dir = NORMALIZE(_worldToLocalMatrix->transformPoint(si.point) - Point3F(0., 0., 0.));
                    } else {
                        dir = NORMALIZE(si.point);
                    }

                    Float theta = math::local_coord::dirToTheta(dir);
                    Float phi = math::local_coord::dirToPhi(dir);
                    Point2F ret(theta * math::INV_PI, phi * math::INV_2PI);
                    return ret;
                }

            private:
                bool _identity = false;
                std::shared_ptr<transform::Transform> _modelViewMatrix = nullptr;
                std::shared_ptr<transform::Transform> _worldToLocalMatrix = nullptr;
            };

        }
    }
}
#endif //KAGUYA_SPHERICALMAPPING2D_H
