//
// Created by Storm Phoenix on 2020/10/15.
//

#ifndef KAGUYA_CORE_H
#define KAGUYA_CORE_H

#define RGB_SPECTRUM

#include <kaguya/common.h>
#include <kaguya/math/Math.h>
#include <memory>

namespace RENDER_NAMESPACE {
    namespace tracer {
        class Camera;
    }

    namespace scene {
        class Scene;
        class Geometry;
    }

    namespace core {

        template<int SpectrumSamples>
        class SpectrumTemplate;

        class RGBSpectrum;

#ifdef RGB_SPECTRUM
        typedef RGBSpectrum Spectrum;
        constexpr int SPECTRUM_CHANNEL = 3;
#endif

        class Light;

        class EnvironmentLight;

        namespace bsdf {
            class BSDF;

            class BXDF;
        }

        namespace bssrdf {
            class BSSRDF;
        }

        constexpr static int MAX_BXDF_NUM = 5;
    }
}

#endif //KAGUYA_CORE_H
