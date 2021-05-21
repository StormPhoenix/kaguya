//
// Created by Storm Phoenix on 2020/12/20.
//

#include <kaguya/core/medium/GridDensityMedium.h>
#include <kaguya/core/phase/HenyeyGreensteinFunction.h>
#include <kaguya/scene/accumulation/AABB.h>

namespace RENDER_NAMESPACE {
    namespace core {
        namespace medium {

            using kaguya::scene::acc::AABB;
            using kaguya::core::HenyeyGreensteinFunction;

            GridDensityMedium::GridDensityMedium(const Spectrum &sigma_a, const Spectrum &sigma_s,
                                                 Float g, int axisXGrid, int axisYGrid, int axisZGrid,
                                                 Float *densities, std::shared_ptr<Transform> transformMatrix) :
                    _sigma_a(sigma_a), _sigma_s(sigma_s),
                    _g(g), _gridX(axisXGrid), _gridY(axisYGrid), _gridZ(axisZGrid),
                    _densities(densities), _transformMatrix(transformMatrix),
                    _invTransformMatrix(transformMatrix->inverse().ptr()) {
                _sigma_t = (_sigma_a + _sigma_s)[0];

                assert(Spectrum(_sigma_t) == (_sigma_a + _sigma_s));

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

            core::Spectrum GridDensityMedium::transmittance(const Ray &ray, Sampler *sampler) const {
                // transform ray from world space to medium space
                const Vector3F origin = _invTransformMatrix->transformPoint(ray.getOrigin());
                const Vector3F dir = _invTransformMatrix->transformVector(ray.getDirection());
                Ray transformedRay = Ray(origin, dir, ray.getMedium());

                // calculate intersection
                AABB aabb(Vector3F(0, 0, 0), Vector3F(1, 1, 1));
                Float minStep = 0, maxStep = transformedRay.getStep();
                bool foundIntersection = aabb.insectPoint(transformedRay, &minStep, &maxStep);
                if (!foundIntersection) {
                    return Spectrum(1.0);
                }

                // loop for sampling step and calculate transmittance
                Float step = minStep;
                Float transmittance = 1.0;
                while (true) {
                    // Sample a travel distance
                    step -= std::log(1 - sampler->sample1D()) / (_sigma_t * _maxDensity);

                    // If ray > maxStep then break
                    if (step >= maxStep) {
                        break;
                    } else {
                        Float d = density(transformedRay.at(step));
                        transmittance *= 1. - std::max(Float(0.), (Float) d * _maxInvDensity);
                    }
                }
                return Spectrum(transmittance);
            }


            core::Spectrum GridDensityMedium::sampleInteraction(const tracer::Ray &ray, Sampler *sampler,
                                                                MediumInteraction *mi, MemoryArena &memoryArena) const {
                // transform ray from world space to medium space
                const Vector3F origin = _invTransformMatrix->transformPoint(ray.getOrigin());
                const Vector3F dir = _invTransformMatrix->transformVector(ray.getDirection());
                Ray transformedRay = Ray(origin, dir, ray.getMedium());
                transformedRay.setStep(ray.getStep());

                // calculate intersection
                AABB aabb(Vector3F(0, 0, 0), Vector3F(1, 1, 1));

                Float minStep = 0, maxStep = transformedRay.getStep();
                bool foundIntersection = aabb.insectPoint(transformedRay, &minStep, &maxStep);
                if (!foundIntersection) {
                    return Spectrum(1.);
                } else {
                    // loop for sampling interaction and transmittance
                    Float step = minStep;
                    while (true) {
                        // Sample a travel distance
                        step -= std::log(1 - sampler->sample1D()) / (_sigma_t * _maxDensity);

                        // Is ray hit AABB boundary ?
                        if (step >= maxStep) {
                            return Spectrum(1.);
                        } else {
                            float d = density(transformedRay.at(step));
                            // Sample by probability
                            if (std::max(Float(0.), (Float) d * _maxInvDensity)
                                > sampler->sample1D()) {
                                (*mi) = MediumInteraction(ray.at(step), -ray.getDirection(), this,
                                                          ALLOC(memoryArena, HenyeyGreensteinFunction)(_g));
                                return _sigma_s / _sigma_t;
                            }
                        }
                    }
                }
            }

            float GridDensityMedium::d(const Vector3F &coord) const {
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

            float GridDensityMedium::density(const Vector3F &coord) const {
                Vector3F coordf = Vector3F(coord.x * _gridX - 0.5, coord.y * _gridY - 0.5, coord.z * _gridZ - 0.5);
                Vector3F coordi = Vector3F(std::floor(coordf.x), std::floor(coordf.y), std::floor(coordf.z));
                Vector3F interValue = coordf - coordi;

                /* Interpolation for x-axis */
                // (x, y, z) - (x + 1, y, z)
                float x1 = math::linearInterpolation(interValue.x, d(coordi), d(coordi + Vector3F(1, 0, 0)));
                // (x, y + 1, z) - (x + 1, y + 1, z)
                float x2 = math::linearInterpolation(interValue.x, d(coordi + Vector3F(0, 1, 0)),
                                                     d(coordi + Vector3F(1, 1, 0)));
                // (x, y, z + 1) - (x + 1, y, z + 1)
                float x3 = math::linearInterpolation(interValue.x, d(coordi + Vector3F(0, 0, 1)),
                                                     d(coordi + Vector3F(1, 0, 1)));
                // (x, y + 1, z + 1) - (x + 1, y + 1, z + 1)
                float x4 = math::linearInterpolation(interValue.x, d(coordi + Vector3F(0, 1, 1)),
                                                     d(coordi + Vector3F(1, 1, 1)));

                /* Interpolation for y-axis */
                // x1 - x2
                float y1 = math::linearInterpolation(interValue.y, x1, x2);
                // x3 - x4
                float y2 = math::linearInterpolation(interValue.y, x3, x4);
                /* Interpolation for z-axis */
                return math::linearInterpolation(interValue.z, y1, y2);
            }
        }
    }
}