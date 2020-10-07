//
// Created by Storm Phoenix on 2020/10/1.
//

#ifndef KAGUYA_CONSTANTTEXTURE_H
#define KAGUYA_CONSTANTTEXTURE_H

#include <kaguya/material/Texture.h>

namespace kaguya {
    namespace material {

        class ConstantTexture : public Texture {
        public:
            ConstantTexture();

            ConstantTexture(const Vector3 &albedo);

            Vector3 sample(double u, double v) override;

        private:
            Vector3 _albedo;
        };

    }
}

#endif //KAGUYA_CONSTANTTEXTURE_H
