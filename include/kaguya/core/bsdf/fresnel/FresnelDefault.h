//
// Created by Storm Phoenix on 2020/10/17.
//

#ifndef KAGUYA_FRESNELDEFAULT_H
#define KAGUYA_FRESNELDEFAULT_H

#include <kaguya/core/bsdf/fresnel/Fresnel.h>

namespace kaguya {
    namespace core {
        namespace bsdf {
            namespace fresnel {
                class FresnelDefault : public Fresnel {
                public:
                    FresnelDefault() {}

                    Spectrum fresnel(Float cosineTheta = 0) const override {
                        // 默认返回 1.0 表示反射概率为 1
                        return Spectrum(1.0);
                    }
                };
            }
        }
    }
}

#endif //KAGUYA_FRESNELDEFAULT_H
