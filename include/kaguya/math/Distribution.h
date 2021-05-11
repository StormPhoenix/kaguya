//
// Created by Storm Phoenix on 2021/5/11.
//

#ifndef KAGUYA_DISTRIBUTION_H
#define KAGUYA_DISTRIBUTION_H

#include <kaguya/Common.h>
#include <kaguya/core/Core.h>
#include <kaguya/sampler/Sampler.h>

namespace kaguya {
    namespace math {
        namespace sampling {

            using sampler::Sampler;

            class Distribution1D {
            public:
                Distribution1D(Float *func, int len);

                Float sampleContinuous(Float *pdf, int *offset, Sampler *sampler) const;

                Float pdfDiscrete(int index) const;

                int length() const;

                friend class Distribution2D;

            private:
                Float _funcIntegration;
                std::vector<Float> _function;
                std::vector<Float> _cdf;
            };

            class Distribution2D {
            public:
                Distribution2D(Float *func2D, int width, int height);

                Point2F sampleContinuous(Float *pdf, Sampler *sampler) const;

                Float pdfContinuous(const Point2F &uv) const;

            private:
                std::vector<std::unique_ptr<Distribution1D>> _rowDistribution;
                std::unique_ptr<Distribution1D> _marginalDistribution;
                int _width, _height;
            };
        }
    }
}

#endif //KAGUYA_DISTRIBUTION_H
