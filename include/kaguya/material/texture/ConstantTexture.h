//
// Created by Storm Phoenix on 2020/10/1.
//

#ifndef KAGUYA_CONSTANTTEXTURE_H
#define KAGUYA_CONSTANTTEXTURE_H

#include <kaguya/core/Core.h>
#include <kaguya/core/spectrum/Spectrum.hpp>
#include <kaguya/material/texture/Texture.h>

namespace RENDER_NAMESPACE {
    namespace material {
        namespace texture {

            template<typename T>
            class ConstantTexture : public Texture<T> {
            public:
                ConstantTexture() {
                    _albedo = T(0.0);
                }

                ConstantTexture(const T &albedo) : _albedo(albedo) {}

                virtual T evaluate(const SurfaceInteraction &si) override {
                    return _albedo;
                }

            private:
                T _albedo;
            };

        }
    }
}

#endif //KAGUYA_CONSTANTTEXTURE_H
