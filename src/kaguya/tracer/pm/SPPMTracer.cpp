//
// Created by Storm Phoenix on 2021/4/8.
//

#include <kaguya/Config.h>
#include <kaguya/parallel/AtomicFloat.h>
#include <kaguya/sampler/SamplerFactory.hpp>
#include <kaguya/tracer/pm/SPPMTracer.h>

namespace kaguya {
    namespace tracer {

        using namespace parallel;

        // TODO VisiblePoint struct

        // Visible Point
        typedef struct VisiblePoint {
            VisiblePoint() {}

            VisiblePoint(const Point3F &p, const Vector3F &wo,
                         const BSDF *bsdf, const Spectrum &beta)
                    : p(p), wo(wo), bsdf(bsdf), beta(beta) {}

            Vector3F wo;
            Point3F p;
            const BSDF *bsdf = nullptr;
            Spectrum beta;
        } VisiblePoint;

        // SPPMPixel
        typedef struct SPPMPixel {
            SPPMPixel() {
                M = 0;
                searchRadius = 0;
            }

            Float searchRadius;

            VisiblePoint vp;

            // M, new photons
            std::atomic<int> M;

            // N, total photons
            Float N = 0.;

            // Phi
            AtomicFloat phi[SPECTRUM_CHANNEL];

            // Accumulated irradiance
            Spectrum tau;

            // Direct light irradiance
            Spectrum Ld;
        } SPPMPixel;

        typedef struct SPPMPixelNode {
            SPPMPixel *pixel;
            SPPMPixelNode *next = nullptr;

            SPPMPixelNode(SPPMPixel *pixel) : pixel(pixel) {
                next = nullptr;
            }
        } SPPMPixelNode;

        bool indexOfGrid(const Point3F &p, const Bound3 &bound, const int gridRes[3],
                         const int gridSize, Point3I *index) {
            bool inside = true;
            Vector3F offset = bound.offset(p);
            for (int axis = 0; axis < 3; axis++) {
                (*index)[axis] = int(offset[axis] / gridSize);
                inside &= (*index)[axis] >= 0 && (*index)[axis] < gridRes[axis];
                (*index)[axis] = clamp((*index)[axis], 0, gridRes[axis] - 1);
            }
            return inside;
        }

        inline unsigned int hashOfIndex(const Point3I &p, int hashSize) {
            return (unsigned int) ((p.x * 73856093) ^ (p.y * 19349663) ^
                                   (p.z * 83492791)) % hashSize;
        }

        SPPMTracer::SPPMTracer() {
            _gamma = Config::searchRadiusDecay;
            _samplePerPixels = Config::Tracer::sampleNum;
            _shootPhotonsPerIter = Config::photonPerIteration;
            _maxDepth = Config::Tracer::maxDepth;
            _initialRadius = Config::initialSearchRadius;
        }

        std::function<void(const int, const int, const int, const int, Sampler *)> SPPMTracer::render() {
            auto renderFunc = [this](const int startRow, const int endRow,
                                     const int startCol, const int endCol,
                                     Sampler *sampler) -> void {
                if (startRow > endRow || startCol > endCol) {
                    return;
                }

                int nPixels = (endRow - startRow + 1) * (endCol - startCol + 1);
                // Build pixels buffer
                std::unique_ptr<SPPMPixel[]> pixels(new SPPMPixel[nPixels]);

                // Set initial search radius
                for (int i = 0; i < nPixels; i++) {
                    pixels[i].searchRadius = _initialRadius;
                }

                // For tile index
                int tileX = startCol / Config::tileSize;
                int tileY = startRow / Config::tileSize;
                sampler->forPixel(Point2F(tileY, tileX));

                // Halton sampler for photon tracing
                int nIterations = _samplePerPixels;
                Sampler *haltonSampler = sampler::SamplerFactory::newSimpleHalton(nIterations * _shootPhotonsPerIter);
                haltonSampler->forPixel({0, 0});

                // Loop iterations
                for (int iter = 0; iter < nIterations; iter++) {
                    MemoryArena arena;
                    // Generate camera ray for each pixels
                    {
                        for (int row = startRow; row <= endRow; row++) {
                            for (int col = startCol; col <= endCol; col++) {
                                // 选取 pixel
                                int offsetRow = row - startRow;
                                int offsetCol = col - startCol;
                                SPPMPixel &pixel = pixels[offsetRow * (endCol - startCol + 1) + offsetCol];

                                // Sample camera ray
                                auto u = (col + sampler->sample1D()) / Float(_camera->getResolutionWidth());
                                auto v = (row + sampler->sample1D()) / Float(_camera->getResolutionHeight());
                                Ray cameraRay = _camera->sendRay(u, v);
                                Spectrum beta(1.0f);
                                bool isSpecularBounce = false;

                                // TODO 由于第一次遇到 diffuse material 就 break，那么每次 camera bounce 得到的路径都是固定的 ... 那有什么做 camera bounce 的必要呢

                                // Intersect with scenes
                                for (int bounce = 0; bounce < _maxDepth; bounce++) {
                                    SurfaceInteraction si;
                                    bool foundIntersection = _scene->intersect(cameraRay, si);

                                    // Intersect with volume ?
                                    MediumInteraction mi;
                                    if (cameraRay.getMedium() != nullptr) {
                                        beta *= cameraRay.getMedium()->sampleInteraction(cameraRay, sampler, &mi,
                                                                                         arena);
                                        if (beta.isBlack()) {
                                            break;
                                        }
                                    }

                                    // Check medium interaction
                                    if (mi.isValid()) {
                                        /* Medium intersection */
                                        // Sample direct light
                                        pixel.Ld += beta * sampleDirectLight(_scene, mi, sampler);

                                        Vector3F wo = -cameraRay.getDirection();
                                        Vector3F wi;
                                        mi.getPhaseFunction()->sampleScatter(wo, &wi, sampler);
                                        cameraRay = mi.sendRay(wi);
                                        isSpecularBounce = false;
                                    } else {
                                        // Surface interaction
                                        if (bounce == 0 || isSpecularBounce) {
                                            if (!foundIntersection) {
                                                pixel.vp.beta = 0;
                                                break;
                                            } else if (si.getAreaLight() != nullptr) {
                                                pixel.Ld += si.getAreaLight()->L(si, -si.direction) * beta;
                                            }
                                        }

                                        if (!foundIntersection) {
                                            // Visible point not found
                                            pixel.vp.beta = 0;
                                            break;
                                        }

                                        // Check volume boundary
                                        if (si.getMaterial() == nullptr) {
                                            cameraRay = si.sendRay(cameraRay.getDirection());
                                            bounce--;
                                            continue;
                                        }

                                        si.buildScatteringFunction(arena);
                                        assert(si.bsdf != nullptr);

                                        // Sample from direct light
                                        if (si.bsdf->allIncludeOf(BXDFType(BSDF_ALL & (~BSDF_SPECULAR)))) {
                                            pixel.Ld += beta * sampleDirectLight(_scene, si, sampler);
                                        }

                                        // Judge visible point
                                        bool isDiffuse = si.bsdf->allIncludeOf(BXDFType(BSDF_DIFFUSE
                                                                                        | BSDF_REFLECTION
                                                                                        | BSDF_TRANSMISSION)) > 0;

                                        bool isGlossy = si.bsdf->allIncludeOf(BXDFType(BSDF_GLOSSY
                                                                                       | BSDF_REFLECTION
                                                                                       | BSDF_TRANSMISSION)) > 0;

                                        Vector3F wo = -cameraRay.getDirection();
                                        if (isDiffuse || (isGlossy && (_maxDepth - 1 == bounce))) {
                                            // 尽量选 Diffuse，实在不行就选择 isGlossy
                                            pixel.vp = VisiblePoint(si.point, wo, si.bsdf, beta);
                                            break;
                                        }

                                        /* Keep tracing ray */
                                        if (bounce < _maxDepth - 1) {
                                            Vector3F wi(0.0);
                                            Float samplePdf = 0;
                                            BXDFType sampleType;
                                            Spectrum f = si.bsdf->sampleF(wo, &wi, &samplePdf, sampler,
                                                                          BSDF_ALL, &sampleType);

                                            if (f.isBlack() || samplePdf == 0.) {
                                                break;
                                            }

                                            isSpecularBounce = (sampleType & BSDF_SPECULAR) > 0;

                                            // Update beta
                                            Float cos = ABS_DOT(si.rendering.normal, NORMALIZE(wi));
                                            beta *= f * cos / samplePdf;

                                            // Russian Roultte
                                            if (beta.g() < 0.25) {
                                                Float terminateProb = 1 - std::min(Float(1.), beta.g());
                                                if (sampler->sample1D() < terminateProb) {
                                                    break;
                                                }
                                                beta /= 1 - terminateProb;
                                            }
                                            cameraRay = si.sendRay(NORMALIZE(wi));
                                        }

                                    }
                                }
                            }
                        }
                    }

                    int gridRes[3];
                    Float gridSize;
                    Bound3 vpsBound;

                    int hashSize = nPixels;
                    std::vector<std::atomic<SPPMPixelNode *>> pixelHashTable(hashSize);

                    /* Grid bound computation */
                    {
                        for (int i = 0; i < nPixels; i++) {
                            const SPPMPixel &pixel = pixels[i];
                            if (pixel.vp.beta.isBlack()) {
                                continue;
                            }

                            // Merge bounds
                            {
                                Bound3 vpBound = {pixel.vp.p};
                                vpBound.expand(pixel.searchRadius);
                                vpBound.merge(vpsBound);
                                vpsBound = vpBound;
                            }

                            // Search max radius
                            gridSize = std::max(gridSize, pixel.searchRadius);
                        }

                        /* Resolution computation */
                        {
                            Vector3F diagonal = vpsBound.diagonal();
                            for (int axis = 0; axis < 3; axis++) {
                                gridRes[axis] = int(diagonal[axis] / gridSize) + 1;
                            }
                        }

                        /* Hash table */
                        {
                            for (int i = 0; i < nPixels; i++) {
                                SPPMPixel &pixel = pixels[i];

                                if (!pixel.vp.beta.isBlack()) {
                                    Float searchRadius = pixel.searchRadius;
                                    Point3F vpMin = pixel.vp.p - searchRadius;
                                    Point3F vpMax = pixel.vp.p + searchRadius;

                                    Point3I vpIdxMin, vpIdxMax;
                                    indexOfGrid(vpMin, vpsBound, gridRes, gridSize, &vpIdxMin);
                                    indexOfGrid(vpMax, vpsBound, gridRes, gridSize, &vpIdxMax);

                                    for (int x = vpIdxMin[0]; x <= vpIdxMax[0]; x++) {
                                        for (int y = vpIdxMin[1]; y <= vpIdxMax[1]; y++) {
                                            for (int z = vpIdxMin[2]; z <= vpIdxMax[2]; z++) {
                                                unsigned int h = hashOfIndex({x, y, z}, hashSize);

                                                SPPMPixelNode *node = ALLOC(arena, SPPMPixelNode)(&pixel);
                                                // TODO 学习 compare_exchange_weak
                                                while (pixelHashTable[h].compare_exchange_weak(
                                                        node->next, node) == false);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    /* Trace photons */
                    {
                        for (int photon = 0; photon < _shootPhotonsPerIter; photon++) {
                            // Uniform sample light
                            Float lightPdf = 0;
                            std::shared_ptr<Light> light = uniformSampleLight(_scene, &lightPdf, haltonSampler);

                            Ray photonRay;
                            Spectrum beta;

                            // Sample photon ray
                            {
                                Vector3F lightNormal;
                                Float pdfPos, pdfDir;
                                Spectrum radiance = light->sampleLe(&photonRay, &lightNormal,
                                                                    &pdfPos, &pdfDir, haltonSampler);
                                if (radiance.isBlack() || pdfPos == 0. || pdfDir == 0.) {
                                    continue;
                                }

                                beta = radiance * ABS_DOT(lightNormal, photonRay.getDirection())
                                       / (pdfPos * pdfDir * lightPdf);
                                if (beta.isBlack()) {
                                    continue;
                                }
                            }

                            SurfaceInteraction si;
                            for (int bounce = 0; bounce < _maxDepth; bounce++) {
                                bool foundIntersection = _scene->intersect(photonRay, si);
                                if (!foundIntersection) {
                                    break;
                                }

                                if (si.getMaterial() == nullptr) {
                                    photonRay = si.sendRay(photonRay.getDirection());
                                    bounce--;
                                    continue;
                                }

                                // Add contribution to visible point
                                {
                                    // Skip first intersection, for reason of indirect light
                                    if (bounce > 0) {
                                        Point3I indexOfPhoton;
                                        indexOfGrid(si.point, vpsBound, gridRes, gridSize, &indexOfPhoton);

                                        unsigned int hash = hashOfIndex(indexOfPhoton, hashSize);
                                        // TODO std::memory_order_relaxed 的用法
                                        for (SPPMPixelNode *node = pixelHashTable[hash].load(std::memory_order_relaxed);
                                             node != nullptr; node = node->next) {
                                            SPPMPixel &pixel = *(node->pixel);

                                            Float searchRadius = pixel.searchRadius;
                                            if (distanceSquare(pixel.vp.p, si.point) >= searchRadius * searchRadius) {
                                                continue;
                                            }

                                            // Contribution computation
                                            Vector3F wi = -photonRay.getDirection();
                                            Spectrum phi = beta * pixel.vp.bsdf->f(pixel.vp.wo, wi, BSDF_ALL);
                                            for (int i = 0; i < SPECTRUM_CHANNEL; i++) {
                                                pixel.phi[i].add(phi[i]);
                                            }
                                            pixel.M++;
                                        }
                                    }
                                }

                                // TODO 考虑 BSSRDF

                                si.buildScatteringFunction(arena);
                                assert(si.bsdf != nullptr);

                                Vector3F wo = -photonRay.getDirection();
                                Vector3F wi;
                                Float samplePdf = 0;

                                Spectrum f = si.bsdf->sampleF(wo, &wi, &samplePdf, haltonSampler, BSDF_ALL, nullptr);
                                if (f.isBlack() || samplePdf == 0.) {
                                    break;
                                }
                                Spectrum newBeta = beta * f * ABS_DOT(wi, si.rendering.normal) / samplePdf;

                                // Russian Roulette
                                Float terminateProb = std::max(1. - newBeta.r() / beta.r(), 0.);
                                if (haltonSampler->sample1D() < terminateProb) {
                                    break;
                                }
                                beta = newBeta / (1 - terminateProb);

                                // Next bounce
                                photonRay = si.sendRay(wi);
                            }

                            haltonSampler->nextSampleRound();
                        }
                    }

                    /* Density estimation */
                    {
                        for (int i = 0; i < nPixels; i++) {
                            SPPMPixel &pixel = pixels[i];

                            if (pixel.M > 0) {
                                Float newN = pixel.N + _gamma * pixel.M;
                                Float totalN = pixel.N + pixel.M;

                                Float oldRadius = pixel.searchRadius;
                                Float newRadius = oldRadius * std::sqrt(newN / totalN);

                                Spectrum phi;
                                for (int i = 0; i < SPECTRUM_CHANNEL; i ++) {
                                    phi[i] = pixel.phi[i];
                                }
                                pixel.tau = (pixel.tau + pixel.vp.beta * phi) * ((newRadius * newRadius) / (oldRadius * oldRadius));
                                pixel.N = newN;
                                pixel.M = 0;
                                pixel.searchRadius = newRadius;

                                for (int i = 0; i < SPECTRUM_CHANNEL; i ++) {
                                    pixel.phi[i] = 0.;
                                }
                            }

                            // Clear visible point
                            pixel.vp.beta = 0.;
                            pixel.vp.bsdf = nullptr;
                        }
                    }

                    /* Write to image buffer */
                    {
                        if (iter == nIterations - 1 || (iter + 1) % _writeFrequence == 0) {
                            for (int row = startRow; row <= endRow; row++) {
                                for (int col = startCol; col <= endCol; col++) {
                                    int offsetRow = row - startRow;
                                    int offsetCol = col - startCol;
                                    SPPMPixel &pixel = pixels[offsetRow * (endCol - startCol + 1) + offsetCol];

                                    // Direct light
                                    Spectrum L = pixel.Ld / (iter + 1);
                                    // Indirect light
                                    Float radius = pixel.searchRadius;
                                    L += pixel.tau / ((iter + 1) * _shootPhotonsPerIter * PI * radius * radius);

                                    _filmPlane->setSpectrum(L, row, col);
                                }
                            }
                        }
                    }

                    arena.clean();
                    sampler->nextSampleRound();
                }

                delete haltonSampler;
            };
            return renderFunc;
        }

    }
}