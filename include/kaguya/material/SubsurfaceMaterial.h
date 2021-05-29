//
// Created by Storm Phoenix on 2021/3/4.
//

#ifndef KAGUYA_SUBSURFACEMATERIAL_H
#define KAGUYA_SUBSURFACEMATERIAL_H

#include <kaguya/common.h>
#include <kaguya/core/bssrdf/TabulatedBSSRDF.h>

namespace RENDER_NAMESPACE {
    namespace material {

        using namespace bssrdf;

        class SubsurfaceMaterial {
        public:
            SubsurfaceMaterial(Spectrum albedoEff, Spectrum mft, Float g, Float eta);

            RENDER_CPU_GPU void evaluateBSDF(SurfaceInteraction &si, MemoryAllocator &allocator,
                                             TransportMode mode = TransportMode::RADIANCE);

            RENDER_CPU_GPU bool isSpecular() {
                return false;
            }

            RENDER_CPU_GPU bool isTwoSided() {
                return _twoSided;
            }

            RENDER_CPU_GPU void setTwoSided(bool twoSided) {
                _twoSided = twoSided;
            }

        private:
            bool _twoSided = true;
            BSSRDFTable _table;
            Spectrum _albedoEff;
            Spectrum _meanFreePath;
            Float _theta;
        };

    }
}

#endif //KAGUYA_SUBSURFACEMATERIAL_H
