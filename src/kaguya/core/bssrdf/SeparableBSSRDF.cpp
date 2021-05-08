//
// Created by Storm Phoenix on 2021/1/10.
//

#include <kaguya/core/bssrdf/SeparableBSSRDF.h>

namespace kaguya {
    namespace core {
        namespace bssrdf {
            using bsdf::BSDF;

            Float fresnelMoment1(Float eta) {
                Float eta2 = eta * eta, eta3 = eta2 * eta, eta4 = eta3 * eta,
                        eta5 = eta4 * eta;
                if (eta < 1)
                    return 0.45966f - 1.73965f * eta + 3.37668f * eta2 - 3.904945 * eta3 +
                           2.49277f * eta4 - 0.68441f * eta5;
                else
                    return -4.61686f + 11.1136f * eta - 10.4646f * eta2 + 5.11455f * eta3 -
                           1.27198f * eta4 + 0.12746f * eta5;
            }

            Float fresnelMoment2(Float eta) {
                Float eta2 = eta * eta, eta3 = eta2 * eta, eta4 = eta3 * eta,
                        eta5 = eta4 * eta;
                if (eta < 1) {
                    return 0.27614f - 0.87350f * eta + 1.12077f * eta2 - 0.65095f * eta3 +
                           0.07883f * eta4 + 0.04860f * eta5;
                } else {
                    Float r_eta = 1 / eta, r_eta2 = r_eta * r_eta, r_eta3 = r_eta2 * r_eta;
                    return -547.033f + 45.3087f * r_eta3 - 218.725f * r_eta2 +
                           458.843f * r_eta + 404.557f * eta - 189.519f * eta2 +
                           54.9327f * eta3 - 9.00603f * eta4 + 0.63942f * eta5;
                }
            }

            SeparableBSSRDF::SeparableBSSRDF(const SurfaceInteraction &po, Material *material, Float theta) :
                    BSSRDF(po), _material(material), _theta(theta) {
                // build local coordinate
                // 构造切线空间
                // 此处的实现和 pbrt 中的实现不同，pbrt 用的是 dudp 和纹理相关，但目前没有实现纹理部分
                // 暂时用入射光线和法线来构造切线空间
                _tanY = NORMALIZE(po.normal);
                _tanZ = NORMALIZE(CROSS(po.direction, _tanY));
                if (!math::isValid(_tanZ)) {
                    math::tangentSpace(_tanY, &_tanX, &_tanZ);
                } else {
                    _tanX = NORMALIZE(CROSS(_tanY, _tanZ));
                }
            }

            Spectrum SeparableBSSRDF::sampleS(std::shared_ptr<Scene> scene, SurfaceInteraction *pi, Float *pdf,
                                              MemoryArena &memoryArena, Sampler *sampler) {
                // Sampling S_p(p_o, p_i)
                Spectrum sp = sampleSp(scene, pi, pdf, memoryArena, sampler);
                if (!sp.isBlack()) {
                    pi->bsdf = ALLOC(memoryArena, BSDF)(*pi);
                    pi->bsdf->addBXDF(ALLOC(memoryArena, SeparableBSSRDFAdapter)(this));
                    // TODO delete why
                    pi->direction = -pi->rendering.normal;
                    pi->wo = pi->rendering.normal;
                }
                return sp;
            }

            Spectrum SeparableBSSRDF::S(const SurfaceInteraction &si, const Vector3F &wi) {
                Float cosTheta = ABS_DOT(po.direction, po.normal);
                Float ft = math::fresnelDielectric(cosTheta, 1, _theta);
                // TODO  wi 是什么坐标系
                return (1 - ft) * Sp(si) * subsurfaceWi(wi);
            }

            Spectrum SeparableBSSRDF::sampleSp(std::shared_ptr<Scene> scene, SurfaceInteraction *pi, Float *pdf,
                                               MemoryArena &memoryArena, Sampler *sampler) {
                // Randomly chose probe ray direction
                Vector3F ry, rx, rz;

                Float u = sampler->sample1D();

                ry = _tanY;
                rx = _tanZ;
                rz = _tanX;

                // TODO 删除冗余代码
                /*
                if (u < 1.0) {
                    ry = _tanY;
                    rx = _tanZ;
                    rz = _tanX;
                    u *= 2;
                } else if (u < 0.75) {
                    rz = _tanY;
                    ry = _tanZ;
                    rx = _tanX;
                    u = (u - 0.5) * 4;
                } else {
                    rx = _tanY;
                    rz = _tanZ;
                    ry = _tanX;
                    u = (u - 0.75) * 4;
                }
                 */

                // Randomly chose spectrum channel, and get Sr
                int channel = math::clamp(int(u * (SPECTRUM_CHANNEL)), 0, SPECTRUM_CHANNEL - 1);
                // reuse u
                u = SPECTRUM_CHANNEL * u - channel;

                Float radius = sampleSr(channel, sampler->sample1D());
                if (radius < 0) {
                    return Spectrum(0);
                }

                // Sample probe ray phi angle
                Float phi = 2 * math::PI * sampler->sample1D();

                Float maxRadius = sampleSr(channel, 0.999);
                if (radius >= maxRadius) {
                    return Spectrum(0.f);
                }

                // Set probe ray range
                Float range = 2 * std::sqrt(std::pow(maxRadius, 2) - std::pow(radius, 2));

                Interaction origin;
                // TODO check ry direction
                origin.point = po.point + radius * (rx * std::cos(phi) + rz * std::sin(phi)) - range * 0.5f * ry;

                Point3F target = origin.point + range * ry;

                // Build intersection chain structure
                struct InteractionChain {
                    SurfaceInteraction si;
                    InteractionChain *next;
                };
                InteractionChain *chain = ALLOC(memoryArena, InteractionChain)();

                // Do intersection
                InteractionChain *pChain = chain;
                int found = 0;

                while (true) {
                    Ray probeRay = origin.sendRayTo(target);

                    if (probeRay.getDirection() == Vector3F(0., 0., 0.) ||
                        !scene->intersect(probeRay, pChain->si)) {
                        break;
                    }

                    origin = pChain->si;
                    // Check same geometry
                    if (pChain->si.getMaterial() == _material) {
                        found++;
                        pChain->next = ALLOC(memoryArena, InteractionChain)();
                        pChain = pChain->next;
                    }
                }

                if (found == 0) {
                    return Spectrum(0);
                }

                // Randomly chose interaction chain
                int chosenIndex = math::clamp(int(found * u), 0, found - 1);
                pChain = chain;
                while (chosenIndex > 0) {
                    pChain = pChain->next;
                    chosenIndex--;
                }
                *pi = pChain->si;

                // Get pdf
                *pdf = SpPdf(*pi) / found;
                return Sp(*pi);
            }

            Spectrum SeparableBSSRDF::Sp(const SurfaceInteraction &si) const {
                return Sr(LENGTH(po.point - si.point));
            }

            Float SeparableBSSRDF::SpPdf(SurfaceInteraction &pi) const {
                Vector3F dir = po.point - pi.point;
                Vector3F dirLocal = Vector3F(DOT(_tanX, dir), DOT(_tanY, dir), DOT(_tanZ, dir));
                Vector3F normalLocal = Vector3F(DOT(_tanX, pi.normal),
                                                DOT(_tanY, pi.normal),
                                                DOT(_tanZ, pi.normal));

                // Channel pdf for _tanX \ _tanY \ _tanZ
                // TODO 删除冗余代码
                Float probAxis[3] = {0., 1.0, 0.};
//                Float probAxis[3] = {0.25, 0.5, 0.25};

                // Radius for three axis
                Float projRadius[3] = {std::sqrt(dirLocal.y * dirLocal.y + dirLocal.z * dirLocal.z),
                                       std::sqrt(dirLocal.x * dirLocal.x + dirLocal.z * dirLocal.z),
                                       std::sqrt(dirLocal.y * dirLocal.y + dirLocal.x * dirLocal.x)};

                // channel pdf
                constexpr Float channelPdf = 1.0 / SPECTRUM_CHANNEL;

                Float pdf = 0;
                // for-loop channel
                for (int ch = 0; ch < SPECTRUM_CHANNEL; ch++) {
                    // for-loop axis
                    for (int axis = 0; axis < 3; axis++) {
                        // product of channel_pdf \ axis_pdf \ radius_pdf \ area_density
                        pdf += channelPdf * probAxis[axis] * SrPdf(ch, projRadius[axis])
                               * std::abs(normalLocal[axis]);
                    }
                }
                return pdf;
            }

            Spectrum SeparableBSSRDF::subsurfaceWi(const Vector3F &wi) const {
                // TODO 参考 Fresnel 积分
                Float c = 1 - 2 * fresnelMoment1(1.0 / _theta);
                return (1 - math::fresnelDielectric(wi.y, 1.0, _theta)) / (c * math::PI);
            }
        }
    }
}