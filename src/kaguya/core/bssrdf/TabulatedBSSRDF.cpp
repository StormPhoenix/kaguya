//
// Created by Storm Phoenix on 2021/3/6.
//

#include <kaguya/core/bssrdf/TabulatedBSSRDF.h>
#include <kaguya/core/bsdf/fresnel/Fresnel.h>

namespace RENDER_NAMESPACE {
    namespace core {
        namespace bssrdf {
            using namespace bsdf;
            using namespace math::spline_curve;

            Float beamDiffusionForSS(Float sigma_s, Float sigma_a, Float g, Float theta,
                                     Float radius) {
                Float sigma_t = sigma_a + sigma_s, rho = sigma_s / sigma_t;

                // 低于 tCrit 的单次散射会导致全反射
                Float tCrit = radius * std::sqrt(theta * theta - 1);
                Float Ess = 0;

                const int nSamples = 100;
                for (int i = 0; i < nSamples; ++i) {
                    // Evaluate single scattering integrand and add to _Ess_
                    Float ti = tCrit - std::log(1 - (i + .5f) / nSamples) / sigma_t;

                    // Determine length $d$ of connecting segment and $\cos\theta_\roman{o}$
                    Float d = std::sqrt(radius * radius + ti * ti);
                    Float cosThetaO = ti / d;

                    // Add contribution of single scattering at depth $t$
                    Ess += rho * std::exp(-sigma_t * (d + tCrit)) / (d * d) *
                           math::phaseFuncHG(cosThetaO, g) * (1 - fresnel::fresnelDielectric(-cosThetaO, 1, theta)) *
                           std::abs(cosThetaO);
                }
                return Ess / nSamples;
            }

            Float beamDiffusionForMS(Float sigma_s, Float sigma_a, Float g, Float theta, Float radius) {
                /**
                 * Reference: http://www.pbr-book.org/3ed-2018/Light_Transport_II_Volume_Rendering/Subsurface_Scattering_Using_the_Diffusion_Equation.html#InvertCatmullRom
                 */
                const int nSamples = 100;
                Float Ed = 0;

                // Principal of similarity
                Float sigma_s_prime = sigma_s * (1 - g);
                Float sigma_t_prime = sigma_a + sigma_s_prime;
                Float albedo_prime = sigma_s_prime / sigma_t_prime;

                // Non-classical diffusion coefficient D
                Float D_g = (2 * sigma_a + sigma_s_prime) / (3 * sigma_t_prime * sigma_t_prime);

                // Effective transport coefficient
                Float sigma_tr = std::sqrt(sigma_a / D_g);

                Float fm1 = fresnelMoment1(theta), fm2 = fresnelMoment2(theta);

                // Dipole solution
                Float ze = -2 * D_g * (1 + 3 * fm2) / (1 - 2 * fm1);

                // Reference: equation 15.31 15.32
                Float cPhi = .25f * (1 - 2 * fm1), cE = .5f * (1 - 3 * fm2);
                for (int i = 0; i < nSamples; ++i) {
                    // Sample real point source depth $\depthreal$
                    Float zr = -std::log(1 - (i + .5f) / nSamples) / sigma_t_prime;

                    // Evaluate dipole integrand $E_{\roman{d}}$ at $\depthreal$ and add to
                    // _Ed_
                    Float zv = -zr + 2 * ze;
                    Float dr = std::sqrt(radius * radius + zr * zr), dv = std::sqrt(radius * radius + zv * zv);

                    // Compute dipole fluence rate $\dipole(r)$ using Equation (15.27)
                    Float phiD = math::INV_4PI / D_g * (std::exp(-sigma_tr * dr) / dr -
                                                        std::exp(-sigma_tr * dv) / dv);

                    // Compute dipole vector irradiance $-\N{}\cdot\dipoleE(r)$ using
                    // Equation (15.27)
                    Float EDn = math::INV_4PI * (zr * (1 + sigma_tr * dr) *
                                                 std::exp(-sigma_tr * dr) / (dr * dr * dr) -
                                                 zv * (1 + sigma_tr * dv) *
                                                 std::exp(-sigma_tr * dv) / (dv * dv * dv));

                    // Add contribution from dipole for depth $\depthreal$ to _Ed_
                    Float E = phiD * cPhi + EDn * cE;
                    Float kappa = 1 - std::exp(-2 * sigma_t_prime * (dr + zr));

                    // First albedo', ratio of the importance sampling weight of the sampling strategy
                    //      an d the sigma_s' in Equation 15.33
                    // Second albedo' is the additional scattering event
                    //      in Grosjean's non-classical monopole in Equation 15.23
                    // Kappa -> Habel et al.(2013) for details
                    Ed += kappa * albedo_prime * albedo_prime * E;
                }
                return Ed / nSamples;
            }

            void beamDiffusionForBSSRDFTable(Float g, Float theta, BSSRDFTable *table) {
                // 填充 radius，小的 radius 会贡献更多的 effective albedo，所以 samples 集中在小的 radius 上
                table->radiusSamples[0] = 0;
                table->radiusSamples[1] = 2.5e-3f;
                for (int i = 2; i < table->nRadiusSamples; i++) {
                    table->radiusSamples[i] = table->radiusSamples[i - 1] * 1.2f;
                }

                /**
                 * 填充 albedo。参考公式 15.48，albedo 也不能均匀采样，因为 albedo 和 effective albedo 不是线性对应的。
                 * 所以采样 albedo 也要选择集中在较大 effective albedo 的 albedo
                 */
                for (int i = 0; i < table->nAlbedoSamples; i++) {
                    table->albedoSamples[i] =
                            (1 - std::exp(-8 * i / (Float) (table->nAlbedoSamples - 1))) /
                            (1 - std::exp(-8));
                }

                // 遍历循环每个 albedo 和 radius，计算对应的 profile
                for (int i = 0; i < table->nAlbedoSamples; i++) {
                    for (int j = 0; j < table->nRadiusSamples; j++) {
                        Float albedo = table->albedoSamples[i];
                        Float radius = table->radiusSamples[j];

                        table->profile[i * table->nRadiusSamples + j] =
                                2 * math::PI * radius * (
                                        // 计算 single scattering term
                                        beamDiffusionForMS(albedo, 1 - albedo, g, theta, radius) +
                                        // 计算 multiple scattering term
                                        beamDiffusionForSS(albedo, 1 - albedo, g, theta, radius));
                    }

                    table->albedoEffSamples[i] =
                            integrateCatmullRom(table->nRadiusSamples, table->radiusSamples.get(),
                                                &table->profile[i * table->nRadiusSamples],
                                                &table->profileCDF[i * table->nRadiusSamples]);
                }
            }

            BSSRDFTable::BSSRDFTable(int nAlbedoSamples, int nRadiusSamples)
                    : nAlbedoSamples(nAlbedoSamples),
                      nRadiusSamples(nRadiusSamples),
                      albedoSamples(new Float[nAlbedoSamples]),
                      radiusSamples(new Float[nRadiusSamples]),
                      profile(new Float[nRadiusSamples * nAlbedoSamples]),
                      albedoEffSamples(new Float[nAlbedoSamples]),
                      profileCDF(new Float[nRadiusSamples * nAlbedoSamples]) {}

            TabulatedBSSRDF::TabulatedBSSRDF(const Spectrum &sigma_a,
                                             const Spectrum &sigma_s,
                                             const BSSRDFTable &table,
                                             const SurfaceInteraction &po,
                                             Material *material, Float theta)
                    : _table(table), SeparableBSSRDF(po, material, theta) {
                _sigma_t = sigma_a + sigma_s;
                for (int ch = 0; ch < SPECTRUM_CHANNEL; ++ch)
                    _albedo[ch] = _sigma_t[ch] != 0 ? (sigma_s[ch] / _sigma_t[ch]) : 0;
            }

            Spectrum TabulatedBSSRDF::Sr(Float radius) const {
                Spectrum Sr(0.f);
                for (int ch = 0; ch < SPECTRUM_CHANNEL; ++ch) {
                    // Convert $r$ into unitless optical radius $r_{\roman{optical}}$
                    Float rOptical = radius * _sigma_t[ch];

                    // Compute spline weights to interpolate BSSRDF on channel _ch_
                    int rhoOffset, radiusOffset;
                    Float rhoWeights[4], radiusWeights[4];
                    if (!catmullRomWeights(_table.nAlbedoSamples, _table.albedoSamples.get(),
                                           _albedo[ch], &rhoOffset, rhoWeights) ||
                        !catmullRomWeights(_table.nRadiusSamples, _table.radiusSamples.get(),
                                           rOptical, &radiusOffset, radiusWeights))
                        continue;

                    // Set BSSRDF value _Sr[ch]_ using tensor spline interpolation
                    Float sr = 0;
                    for (int i = 0; i < 4; ++i) {
                        for (int j = 0; j < 4; ++j) {
                            Float weight = rhoWeights[i] * radiusWeights[j];
                            if (weight != 0)
                                sr += weight *
                                      _table.evaluate(rhoOffset + i, radiusOffset + j);
                        }
                    }

                    // Cancel marginal PDF factor from tabulated BSSRDF profile
                    if (rOptical != 0) sr /= 2 * math::PI * rOptical;
                    Sr[ch] = sr;
                }
                // Transform BSSRDF value into world space units
                Sr *= _sigma_t * _sigma_t;
                return Sr.clamp();
            }

            Float TabulatedBSSRDF::SrPdf(int ch, Float radius) const {
                Float rOptical = radius * _sigma_t[ch];

                int albedoOffset, radiusOffset;
                Float albedoWeights[4], radiusWeights[4];
                if (!catmullRomWeights(_table.nAlbedoSamples, _table.albedoSamples.get(), _albedo[ch],
                                       &albedoOffset, albedoWeights) ||
                    !catmullRomWeights(_table.nRadiusSamples, _table.radiusSamples.get(),
                                       rOptical, &radiusOffset, radiusWeights)) {
                    return 0.f;
                }

                Float sr = 0, rhoEff = 0;
                // Iteration over albedos
                for (int i = 0; i < 4; ++i) {
                    if (albedoWeights[i] == 0) continue;
                    rhoEff += _table.albedoEffSamples[albedoOffset + i] * albedoWeights[i];
                    for (int j = 0; j < 4; ++j) {
                        if (radiusWeights[j] == 0) continue;
                        sr += _table.evaluate(albedoOffset + i, radiusOffset + j) *
                              albedoWeights[i] * radiusWeights[j];
                    }
                }

                if (rOptical != 0) {
                    sr /= 2 * math::PI * rOptical;
                }
                return std::max((Float) 0, sr * _sigma_t[ch] * _sigma_t[ch] / rhoEff);
            }

            Float TabulatedBSSRDF::sampleSr(int ch, Float sample) const {
                if (_sigma_t[ch] == 0) {
                    return -1;
                }

                return sampleCatmullRom2D(_table.nAlbedoSamples, _table.nRadiusSamples,
                                          _table.albedoSamples.get(), _table.radiusSamples.get(),
                                          _table.profile.get(), _table.profileCDF.get(),
                                          _albedo[ch], sample) / _sigma_t[ch];
            }
        }
    }
}