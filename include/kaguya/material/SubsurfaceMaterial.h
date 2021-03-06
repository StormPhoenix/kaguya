//
// Created by Storm Phoenix on 2021/3/4.
//

#ifndef KAGUYA_SUBSURFACEMATERIAL_H
#define KAGUYA_SUBSURFACEMATERIAL_H

#include <kaguya/core/bssrdf/TabulatedBSSRDF.h>
#include <kaguya/material/Material.h>

namespace kaguya {
    namespace material {

        using namespace bssrdf;

        class SubsurfaceMaterial : public Material {
        public:
            SubsurfaceMaterial(Spectrum albedoEff, Spectrum mft, Float g, Float eta);

            virtual void computeScatteringFunctions(SurfaceInteraction &si, MemoryArena &memoryArena,
                               TransportMode mode = TransportMode::RADIANCE) override;

        private:
            BSSRDFTable _table;
            Spectrum _albedoEff;
            Spectrum _meanFreePath;
            Float _theta;
        };

    }
}

#endif //KAGUYA_SUBSURFACEMATERIAL_H
