//
// Created by Storm Phoenix on 2021/4/15.
//

#ifndef KAGUYA_UVMAPPING2D_H
#define KAGUYA_UVMAPPING2D_H

#include <kaguya/material/texture/TextureMapping2D.h>

namespace RENDER_NAMESPACE {
    namespace material {
        namespace texture {
            using namespace core;

            class UVMapping2D : public TextureMapping2D {
            public:
                UVMapping2D(Float uScale = 1.0, Float vScale = 1.0) :
                        _uScale(uScale), _vScale(vScale) {}

                virtual Point2F map(const SurfaceInteraction &si) override {
                    return Point2F(si.u * _uScale, si.v * _vScale);
                }

            private:
                Float _uScale;
                Float _vScale;
            };

        }
    }
}
#endif //KAGUYA_UVMAPPING2D_H
