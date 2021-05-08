//
// Created by Storm Phoenix on 2021/3/6.
//

#include <kaguya/math/Math.h>
#include <kaguya/core/bsdf/BXDFSpecular.h>
#include <kaguya/material/SubsurfaceMaterial.h>

namespace kaguya {
    namespace material {

        using kaguya::core::bsdf::BXDFSpecular;

        SubsurfaceMaterial::SubsurfaceMaterial(Spectrum albedoEff, Spectrum mft, Float g, Float eta)
                : _theta(eta), _albedoEff(albedoEff), _meanFreePath(mft), _table(100, 64) {
            beamDiffusionForBSSRDFTable(g, eta, &_table);
        }

        /**
         * 从 BSSRDFTable 中恢复出 BSSRDF 需要的 sigma_s 和 sigma_a. 对于使用者很方便，仅仅需要指定
         * effective albedo 和 sigma_t，就可以推测出 sigma_s 和 sigma_a。计算的方法是对 table 中 albedo 和 effective albedo
         * 的函数关系求逆。
         * @param table
         * @param albedoEff
         * @param meanFreePath
         * @param sigma_a
         * @param sigma_s
         */
        void subsurfaceFromDiffuse(const BSSRDFTable &table, const Spectrum &albedoEff,
                                   const Spectrum &meanFreePath, Spectrum *sigma_a,
                                   Spectrum *sigma_s) {

            for (int ch = 0; ch < SPECTRUM_CHANNEL; ch++) {
                Float rho = math::spline_curve::invertCatmullRom(
                        table.nAlbedoSamples, table.albedoSamples.get(),
                        table.albedoEffSamples.get(), albedoEff[ch]);
                (*sigma_s)[ch] = rho / meanFreePath[ch];
                (*sigma_a)[ch] = (1 - rho) / meanFreePath[ch];
            }
        }

        void SubsurfaceMaterial::computeScatteringFunctions(SurfaceInteraction &si, MemoryArena &memoryArena, TransportMode mode) {

            si.bsdf = ALLOC(memoryArena, BSDF)(si);
            si.bsdf->addBXDF(ALLOC(memoryArena, BXDFSpecular)(1.0f, 1.0f, _theta, mode));

            Spectrum sigma_a, sigma_s;
            subsurfaceFromDiffuse(_table, _albedoEff, _meanFreePath, &sigma_a, &sigma_s);
            si.bssrdf = ALLOC(memoryArena, TabulatedBSSRDF)(sigma_a, sigma_s, _table, si, this, _theta);
        }
    }
}