//
// Created by Storm Phoenix on 2021/4/15.
//

#ifndef KAGUYA_UVMAPPING2D_H
#define KAGUYA_UVMAPPING2D_H

#include <kaguya/material/texture/TextureMapping2D.h>

namespace kaguya {
    namespace material {
        namespace texture {
            using namespace core;

            class UVMapping2D : public TextureMapping2D {
            public:
                UVMapping2D() {}

                virtual Point2F map(const SurfaceInteraction &si) override {
                    return Point2F(si.u, si.v);
                }
            };

        }
    }
}
#endif //KAGUYA_UVMAPPING2D_H
