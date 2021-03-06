//
// Created by Storm Phoenix on 2021/3/6.
//

#include <kaguya/core/bssrdf/TabulatedBSSRDF.h>

namespace kaguya {
    namespace core {
        namespace bssrdf {

            using namespace kaguya::math::spline_curve;

            Spectrum TabulatedBSSRDF::Sr(Float radius) const {
                Spectrum ret(0);
                // Iteration over channels
                for (int ch = 0; ch < SPECTRUM_CHANNEL; ch++) {
                    // Convert radius into unitless optical radius, optical radius = radius / (mean free path)
                    Float opticalR = radius * _sigma_t[ch];

                    int albedoOffset, radiusOffset;
                    Float albedoWeights[4], radiusWeights[4];

                    // Compute spline weight
                    if (!catmullRomWeights(_table.nAlbedoSamples, _table.albedoSamples.get(), _albedo[ch],
                                           &albedoOffset, albedoWeights) ||
                        !catmullRomWeights(_table.nRadiusSamples, _table.radiusSamples.get(), opticalR,
                                           &radiusOffset, radiusWeights)) {
                        continue;
                    }

                    Float sr = 0;
                    // Iteration over albedo samples
                    for (int i = 0; i < 4; i++) {
                        // Iteration over radius samples
                        for (int j = 0; j < 4; j++) {
                            Float weight = albedoWeights[i] * radiusWeights[j];
                            if (weight != 0) {
                                sr += weight * _table.evaluate(albedoOffset + i, radiusOffset + j);
                            }
                        }
                    }

                    if (opticalR != 0) {
                        sr /= 2 * kaguya::math::PI * opticalR;
                    }
                    ret[ch] = sr;
                }
                ret *= _sigma_t * _sigma_t;
                return ret.clamp();
            }

            Float TabulatedBSSRDF::SrPdf(int ch, Float radius) const {
                Float opticalR = radius * _sigma_t[ch];

                int albedoOffset, radiusOffset;
                Float albedoWeights[4], radiusWeights[4];

                if (!catmullRomWeights(_table.nAlbedoSamples, _table.albedoSamples.get(), _albedo[ch],
                                       &albedoOffset, albedoWeights) ||
                    !catmullRomWeights(_table.nRadiusSamples, _table.radiusSamples.get(), opticalR,
                                       &radiusOffset, radiusWeights)) {
                    return 0.f;
                }

                Float albedoEff = 0, sr = 0;
                // Iteration over albedos
                for (int i = 0; i < 4; i++) {
                    if (albedoWeights[i] == 0) {
                        continue;
                    }
                    albedoEff += _table.albedoEffSamples[albedoOffset + i] * albedoWeights[i];
                    // Iteration over radius
                    for (int j = 0; j < 4; j++) {
                        if (radiusWeights[j] == 0) {
                            continue;
                        }
                        sr += _table.evaluate(albedoOffset + i, radiusOffset + j)
                              * albedoWeights[i] * radiusWeights[j];
                    }
                }

                if (opticalR != 0) {
                    sr /= 2 * math::PI * opticalR;
                }

                return std::max((Float) 0, sr * _sigma_t[ch] * _sigma_t[ch] / albedoEff);
            }

            Float TabulatedBSSRDF::sampleSr(int ch, Float sample) const {
                if (_sigma_t[sample] == 0) {
                    return -1;
                }

                return sampleCatmullRom2D(_table.nAlbedoSamples, _table.albedoSamples.get(),
                                          _table.nRadiusSamples, _table.radiusSamples.get(),
                                          _table.profile.get(), _table.profileCDF.get(),
                                          _albedo[ch], sample) / _sigma_t[ch];
            }
        }
    }
}