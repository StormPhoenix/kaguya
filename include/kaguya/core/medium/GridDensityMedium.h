//
// Created by Storm Phoenix on 2020/12/20.
//

#ifndef KAGUYA_GRIDDENSITYMEDIUM_H
#define KAGUYA_GRIDDENSITYMEDIUM_H

#include <kaguya/core/Transform.h>
#include <kaguya/core/medium/Medium.h>

namespace kaguya {
    namespace core {
        namespace medium {

            using core::transform::Transform;

            class GridDensityMedium : public Medium {
            public:
                GridDensityMedium(const Spectrum &absorptionSigma,
                                  const Spectrum &scatteringSigma,
                                  Float g, int axisXGrid, int axisYGrid, int axisZGrid,
                                  Float *densities,
                                  std::shared_ptr<Transform> transformMatrix = std::make_shared<Transform>());

                virtual core::Spectrum transmittance(const Ray &ray, Sampler *sampler1D) const override;

                virtual core::Spectrum sampleInteraction(const tracer::Ray &ray,
                                                         Sampler *sampler1D,
                                                         MediumInteraction *mi,
                                                         MemoryArena &memoryArena) const override;

                ~GridDensityMedium();

            private:
                /**
                 * Trilinearly interpolate densities
                 * @param coordinate
                 * @return
                 */
                float density(const Vector3F &coord) const;

                /**
                 * Index from _densities
                 * @param coord
                 * @return
                 */
                float d(const Vector3F &coord) const;

            private:
                const Spectrum _absorptionSigma;
                const Spectrum _scatteringSigma;
                Float _totalSigma;
                Float _g;
                float *_densities = nullptr;
                std::shared_ptr<Transform> _transformMatrix = nullptr;
                std::shared_ptr<Transform> _invTransformMatrix = nullptr;
                float _maxInvDensity;
                float _maxDensity;
                int _gridX;
                int _gridY;
                int _gridZ;
            };
        }
    }
}

#endif //KAGUYA_GRIDDENSITYMEDIUM_H
