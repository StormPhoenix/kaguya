//
// Created by Storm Phoenix on 2021/1/10.
//

#include <kaguya/core/bsdf/BSDF.h>
#include <kaguya/core/bssrdf/SeparableBSSRDF.h>

namespace kaguya {
    namespace core {

        Spectrum
        SeparableBSSRDF::sampleS(const Scene &scene, SurfaceInteraction *si, double *pdf, MemoryArena &memoryArena,
                                 Sampler *sampler) {
            // Sampling S_p(p_o, p_i)
            Spectrum sp = sampleSubsurfacePoint(scene, si, pdf, memoryArena, sampler);
            if (!sp.isBlack()) {
                si->setBSDF(ALLOC(memoryArena, BSDF)(*si));
                si->getBSDF()->addBXDF(ALLOC(memoryArena, SeparableBSSRDFAdapter)(this));
                si->setDirection(po.getDirection());
            }
            return sp;
        }

        Spectrum SeparableBSSRDF::sampleSubsurfacePoint(const Scene &scene, SurfaceInteraction *si, double *pdf,
                                                        MemoryArena &memoryArena, Sampler *sampler) {
            // Randomly chose probe ray direction
            Vector3d ry, rx, rz;

            double u = sampler->sample1D();
            if (u < 0.5) {
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

            // Randomly chose spectrum channel, and get Sr
            int spectrumChannel = math::clamp(int(u * (SPECTRUM_CHANNEL)), 0, SPECTRUM_CHANNEL - 1);
            // reuse u
            u = SPECTRUM_CHANNEL * u - spectrumChannel;

            double radius = sampleSubsurfaceRadius(spectrumChannel, u);
            if (radius < 0) {
                return Spectrum(0);
            }

            // Sample probe ray phi angle
            double phi = 2 * math::PI * sampler->sample1D();

            double maxRadius = sampleSubsurfaceRadius(spectrumChannel, 0.999);
            // Set probe ray range
            double range = 2 * std::sqrt(std::pow(maxRadius, 2) - std::pow(radius, 2));

            Interaction origin;
            // TODO check ry direction
            origin.setPoint(po.getPoint() + radius * (rx * std::cos(phi) + rz * std::sin(phi)) + range * 0.5 * ry);
            Point3d target = origin.getPoint() - range * ry;

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
                if (probeRay.getDirection() == Vector3d(0., 0., 0.) ||
                    !scene.intersect(probeRay, pChain->si)) {
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
            *si = pChain->si;

            // Get pdf
            *pdf = subsurfacePointPdf(*si) / found;
            return subsurfacePoint(*si);
        }

        Spectrum SeparableBSSRDF::subsurfacePoint(SurfaceInteraction &si) const {
            return subsurfaceRadius(LENGTH(po.getPoint() - si.getPoint()));
        }
    }
}