//
// Created by Storm Phoenix on 2021/3/5.
//

#ifndef KAGUYA_TABULATEDBSSRDF_H
#define KAGUYA_TABULATEDBSSRDF_H

#include <kaguya/core/bssrdf/SeparableBSSRDF.h>

namespace kaguya {
    namespace core {
        namespace bssrdf {

            // Sr(albedo, radius, sigma_t, g, refraction)
            typedef struct BSSRDFTable {
                const int nAlbedoSamples;
                std::unique_ptr<Float[]> albedoSamples;
                std::unique_ptr<Float[]> albedoEffSamples;

                const int nRadiusSamples;
                std::unique_ptr<Float[]> radiusSamples;

                // nAlbedoSamples x nRadiusSamples
                std::unique_ptr<Float[]> profile;

                // nAlbedoSamples x nRadiusSamples
                std::unique_ptr<Float[]> profileCDF;

                BSSRDFTable(int nRhoSamples, int nRadiusSamples);

                inline Float evaluate(int albedoIndex, int radiusIndex) const {
                    return profile[albedoIndex * nRadiusSamples + radiusIndex];
                }

            } BSSRDFTable;

            void beamDiffusionForBSSRDFTable(Float g, Float theta, BSSRDFTable *table);

            class TabulatedBSSRDF : public SeparableBSSRDF {
            public:
                TabulatedBSSRDF(const Spectrum &sigma_a,
                                const Spectrum &sigma_s,
                                const BSSRDFTable &table,
                                const SurfaceInteraction &po,
                                Material *material, Float theta);

            protected:
                virtual Spectrum Sr(Float radius) const override;

                virtual Float SrPdf(int ch, Float radius) const override;

                virtual Float sampleSr(int ch, Float sample) const override;

            private:
                const BSSRDFTable &_table;
                Spectrum _sigma_t, _albedo;
            };

        }
    }
}

#endif //KAGUYA_TABULATEDBSSRDF_H
