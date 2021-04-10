//
// Created by Storm Phoenix on 2021/4/8.
//

#include <kaguya/parallel/AtomicFloat.h>
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

            // Phi
            AtomicFloat phi[SPECTRUM_CHANNEL];

            // Accumulated irradiance
            Spectrum tau;

            // Direct light irradiance
            Spectrum Ld;
        } SPPMPixel;

        typedef struct SPPMPixelNode {
            const SPPMPixel *pixel;
            SPPMPixelNode *next = nullptr;

            SPPMPixelNode(const SPPMPixel *pixel) : pixel(pixel) {
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

        /* TODO
         * 每次发射一堆 photons 却只处理一个 visible points，效率非常低下
         *
         * RenderPool 每次只渲染一个 pixel 的结构必须要重构了。这个不会很麻烦，只要将
         * RenderTask 里面的 func2D 修改成 range 形式即可（现在是按照 pixel-wise 的方式渲染的）
         **/
        std::function<void(const int, const int, const int, const int, Sampler *)> SPPMTracer::render() {
            // TODO 创建保存 3D-Grid 的 HashMap 数据结构

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

                // Loop iterations
                int nIterations = _samplePerPixels;
                for (int iter = 0; iter < nIterations; iter++) {

                    MemoryArena arena;
                    // Generate camera ray for each pixels
                    {
                        for (int row = startRow; row <= endRow; row++) {
                            for (int col = startCol; col <= endCol; col++) {
                                sampler->forPixel(Point2F(row, col));

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
                                                pixel.Ld +=
                                                        si.getAreaLight()->lightRadiance(si, -si.direction) * beta;
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
                                        if (bounce == _maxDepth - 1) {
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
                    Bound3 boxBound;
                    /* Grid size computation */
                    {
                        Float gridSize = 0;
                        for (int i = 0; i < nPixels; i++) {
                            const SPPMPixel &pixel = pixels[i];
                            if (pixel.vp.beta.isBlack()) {
                                continue;
                            }

                            // Merge bounds
                            {
                                Bound3 vpBound = {pixel.vp.p};
                                vpBound.expand(pixel.searchRadius);
                                vpBound.merge(boxBound);
                                boxBound = vpBound;
                            }

                            // Search max radius
                            gridSize = std::max(gridSize, pixel.searchRadius);
                        }

                        /* Resolution computation */
                        {
                            Vector3F diagonal = boxBound.diagonal();
                            for (int axis = 0; axis < 3; axis++) {
                                gridRes[axis] = int(diagonal[axis] / gridSize) + 1;
                            }
                        }

                        /* Hash table */
                        int hashSize = nPixels;
                        std::vector<std::atomic<SPPMPixelNode *>> pixelHashTable(hashSize);
                        {
                            for (int i = 0; i < nPixels; i++) {
                                const SPPMPixel &pixel = pixels[i];
                                if (!pixel.vp.beta.isBlack()) {
                                    Float searchRadius = pixel.searchRadius;
                                    Point3F vpMin = pixel.vp.p - searchRadius;
                                    Point3F vpMax = pixel.vp.p + searchRadius;

                                    Point3I vpIdxMin, vpIdxMax;
                                    indexOfGrid(vpMin, boxBound, gridRes, gridSize, &vpIdxMin);
                                    indexOfGrid(vpMax, boxBound, gridRes, gridSize, &vpIdxMax);

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

                    // Trace photons
                    {
                        for (int photon = 0; photon < _shootPhotonsPerIter; photon++) {
                            // Uniform sample light
                            Float lightPdf = 0;
                            std::shared_ptr<Light> light = uniformSampleLight(_scene, &lightPdf, sampler);

                            // Sample photon ray
                            // TODO

                            for (int bounce = 0; bounce < _maxDepth; bounce++) {

                            }

                            // Loop depth
                            // Find diffuse material point, and search visible points
                            // 这里需要一个 HashMap 查找附近所有的 visible points

                            // Density estimate
                            // 估算光照，加入到 SPPM Pixel 里
                            // ?? Intensity 是如何体现在 photon 上面的
                            // 每收到一个 paritcle
                            // 增加 M ++
                            // 增加 Phi -> 粒子对 visible point 的贡献

                            // 修改 R （radius）
                            // 修改 Ld
                        }
                    }

                    arena.clean();
                    sampler->nextSampleRound();
                }
            };

            // TODO
            // Writing file plane

            return renderFunc;
        }

    }
}