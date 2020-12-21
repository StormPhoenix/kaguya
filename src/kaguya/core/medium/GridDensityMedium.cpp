//
// Created by Storm Phoenix on 2020/12/20.
//

#include <kaguya/core/medium/GridDensityMedium.h>
#include <kaguya/core/phase/HenyeyGreensteinFunction.h>
#include <kaguya/scene/accumulation/AABB.h>

namespace kaguya {
    namespace core {
        namespace medium {

            using kaguya::scene::acc::AABB;
            using kaguya::core::HenyeyGreensteinFunction;

            GridDensityMedium::GridDensityMedium(const Spectrum &absorptionSigma, const Spectrum &scatteringSigma,
                                                 double g, int axisXGrid, int axisYGrid, int axisZGrid,
                                                 float *densities, std::shared_ptr<Matrix4> transformMatrix) :
                    _absorptionSigma(absorptionSigma), _scatteringSigma(scatteringSigma),
                    _g(g), _gridX(axisXGrid), _gridY(axisYGrid), _gridZ(axisZGrid),
                    _densities(densities), _transformMatrix(transformMatrix) {
                _totalSigma = (_absorptionSigma + _scatteringSigma)[0];

                assert(Spectrum(_totalSigma) == (_absorptionSigma + _scatteringSigma));

                float maxDensity = 0;
                for (int i = 0; i < _gridZ * _gridY * _gridX; i++) {
                    if (maxDensity < _densities[i]) {
                        maxDensity = _densities[i];
                    }
                }
                _maxDensity = maxDensity;
                _maxInvDensity = 1.0 / _maxDensity;
            }

            GridDensityMedium::~GridDensityMedium() {
                delete _densities;
            }

            core::Spectrum GridDensityMedium::transmittance(const tracer::Ray &ray, const Sampler1D *sampler1D) const {
                // transform ray from world space to medium space
                const Vector3 origin = _transformMatrix != nullptr ?
                                       INVERSE(*_transformMatrix) * Vector4(ray.getOrigin(), 1.0) : ray.getOrigin();

                const Vector3 dir = _transformMatrix != nullptr ?
                                    INVERSE(*_transformMatrix) * Vector4(ray.getDirection(), 0.0) : ray.getDirection();
                Ray transformedRay = Ray(origin, dir, ray.getMedium());

                // calculate intersection
                AABB aabb(Vector3(0, 0, 0), Vector3(1, 1, 1));
                double minStep = 0, maxStep = transformedRay.getStep();
                bool foundIntersection = aabb.insectPoint(transformedRay, &minStep, &maxStep);
                if (!foundIntersection) {
                    return Spectrum(1.0);
                }

                // loop for sampling step and calculate transmittance
                double step = minStep;
                double transmittance = 1.0;
                while (true) {
                    // Sample a travel distance
                    step -= std::log(1 - sampler1D->sample()) / (_totalSigma * _maxDensity);

                    // If ray > maxStep then break
                    if (step >= maxStep) {
                        break;
                    } else {
                        float d = density(transformedRay.at(step));
                        transmittance *= 1. - std::max(0., (double) d * _maxInvDensity);
                    }
                }
                return Spectrum(transmittance);
            }


            core::Spectrum GridDensityMedium::sampleInteraction(const tracer::Ray &ray, const Sampler1D *sampler1D,
                                                                MediumInteraction *mi, MemoryArena &memoryArena) const {
                // transform ray from world space to medium space
                const Vector3 origin = _transformMatrix != nullptr ?
                                       INVERSE(*_transformMatrix) * Vector4(ray.getOrigin(), 1.0) : ray.getOrigin();

                const Vector3 dir = _transformMatrix != nullptr ?
                                    INVERSE(*_transformMatrix) * Vector4(ray.getDirection(), 0.0) : ray.getDirection();
                Ray transformedRay = Ray(origin, dir, ray.getMedium());
                transformedRay.setStep(ray.getStep());

                // calculate intersection
                AABB aabb(Vector3(0, 0, 0), Vector3(1, 1, 1));

                double minStep = 0, maxStep = transformedRay.getStep();
                bool foundIntersection = aabb.insectPoint(transformedRay, &minStep, &maxStep);
                if (!foundIntersection) {
                    return Spectrum(1.);
                } else {
                    // loop for sampling interaction and transmittance
                    double step = minStep;
                    while (true) {
                        // Sample a travel distance
                        step -= std::log(1 - sampler1D->sample()) / (_totalSigma * _maxDensity);

                        // Is ray hit AABB boundary ?
                        if (step >= maxStep) {
                            return Spectrum(1.);
                        } else {
                            float d = density(transformedRay.at(step));
                            // Sample by probability
                            if (std::max(0., (double) d * _maxInvDensity)
                                > sampler1D->sample()) {
                                (*mi) = MediumInteraction(ray.at(step), -ray.getDirection(), this,
                                                          ALLOC(memoryArena, HenyeyGreensteinFunction)(_g));
                                return _scatteringSigma / _totalSigma;
                            }
                        }
                    }
                }
            }

            float GridDensityMedium::d(const Vector3 &coord) const {
                assert(_densities != nullptr);

                int x = std::floor(coord.x);
                int y = std::floor(coord.y);
                int z = std::floor(coord.z);

                // check range
                if (x < 0 || x >= _gridX ||
                    y < 0 || y >= _gridY ||
                    z < 0 || z >= _gridZ) {
                    return 0.;
                }

//                z * _gridX * _axisYGrid + y * _gridX + x
                return _densities[(z * _gridY + y) * _gridX + x];
            }

            float GridDensityMedium::density(const Vector3 &coord) const {
                Vector3 coordf = Vector3(coord.x * _gridX - 0.5, coord.y * _gridY - 0.5, coord.z * _gridZ - 0.5);
                Vector3 coordi = Vector3(std::floor(coordf.x), std::floor(coordf.y), std::floor(coordf.z));
                Vector3 interValue = coordf - coordi;

                /* Interpolation for x-axis */
                // (x, y, z) - (x + 1, y, z)
                float x1 = linearInterpolation(interValue.x, d(coordi), d(coordi + Vector3(1, 0, 0)));
                // (x, y + 1, z) - (x + 1, y + 1, z)
                float x2 = linearInterpolation(interValue.x, d(coordi + Vector3(0, 1, 0)),
                                               d(coordi + Vector3(1, 1, 0)));
                // (x, y, z + 1) - (x + 1, y, z + 1)
                float x3 = linearInterpolation(interValue.x, d(coordi + Vector3(0, 0, 1)),
                                               d(coordi + Vector3(1, 0, 1)));
                // (x, y + 1, z + 1) - (x + 1, y + 1, z + 1)
                float x4 = linearInterpolation(interValue.x, d(coordi + Vector3(0, 1, 1)),
                                               d(coordi + Vector3(1, 1, 1)));

                /* Interpolation for y-axis */
                // x1 - x2
                float y1 = linearInterpolation(interValue.y, x1, x2);
                // x3 - x4
                float y2 = linearInterpolation(interValue.y, x3, x4);
                /* Interpolation for z-axis */
                return linearInterpolation(interValue.z, y1, y2);
            }
        }
    }
}