//
// Created by Storm Phoenix on 2021/5/11.
//

#ifndef KAGUYA_CHESSBOARDTEXTURE_H
#define KAGUYA_CHESSBOARDTEXTURE_H

#include <kaguya/material/texture/Texture.h>
#include <kaguya/material/texture/UVMapping2D.h>

namespace kaguya {
    namespace material {
        namespace texture {

            template<typename T>
            class ChessboardTexture : public Texture<T> {
            public:
                ChessboardTexture(const T &color1, const T &color2, Float uScale, Float vScale);

                virtual T evaluate(const SurfaceInteraction &si) override;

            private:
                TextureMapping2D::Ptr _textureMapping2D;
                T color1;
                T color2;
            };

            template<typename T>
            ChessboardTexture<T>::ChessboardTexture(const T &color1, const T &color2,
                                                    Float uScale, Float vScale) : color1(color1), color2(color2) {
                _textureMapping2D = std::make_shared<UVMapping2D>(uScale, vScale);
            }

            template<typename T>
            T ChessboardTexture<T>::evaluate(const SurfaceInteraction &si) {
                Point2F uv = _textureMapping2D->map(si);
                if ((int(uv[0]) + int(uv[1])) % 2 == 0) {
                    return color1;
                } else {
                    return color2;
                }
            }
        }
    }
}

#endif //KAGUYA_CHESSBOARDTEXTURE_H
