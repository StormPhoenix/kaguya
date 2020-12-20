//
// Created by Storm Phoenix on 2020/12/20.
//

#ifndef KAGUYA_GRIDDENSITYMEDIUM_H
#define KAGUYA_GRIDDENSITYMEDIUM_H

#include <kaguya/core/medium/Medium.h>

namespace kaguya {
    namespace core {
        namespace medium {

            class GridDensityMedium : public Medium {
            public:
                GridDensityMedium(const Spectrum &absorptionSigma,
                                  const Spectrum &scatteringSigma,
                                  double g, int axisXGrid, int axisYGrid, int axisZGrid,
                                  float *densities,
                                  std::shared_ptr<Matrix4> transformMatrix = nullptr);

                virtual core::Spectrum transmittance(const tracer::Ray &ray, const Sampler1D *sampler1D) const override;

                virtual core::Spectrum sampleInteraction(const tracer::Ray &ray,
                                                         const Sampler1D *sampler1D,
                                                         MediumInteraction *mi,
                                                         MemoryArena &memoryArena) const override;

                ~GridDensityMedium();

            private:
                /**
                 * Trilinearly interpolate densities
                 * @param coordinate
                 * @return
                 */
                float density(const Vector3 &coord) const;

                /**
                 * Index from _densities
                 * @param coord
                 * @return
                 */
                float d(const Vector3 &coord) const;

            private:
                const Spectrum _absorptionSigma;
                const Spectrum _scatteringSigma;
                double _totalSigma;
                double _g;
                float *_densities = nullptr;
                std::shared_ptr<Matrix4> _transformMatrix = nullptr;
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
