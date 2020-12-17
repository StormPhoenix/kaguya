//
// Created by Storm Phoenix on 2020/11/18.
//

#ifndef KAGUYA_SAMPLER_HPP
#define KAGUYA_SAMPLER_HPP

#include <functional>
#include <random>

namespace kaguya {
    namespace math {
        namespace random {

            class Sampler1D {
            public:
                Sampler1D() {
                    std::random_device rd;  //Will be used to obtain a seed for the random number engine
                    generator = std::mt19937(rd());
                    distribution = std::uniform_real_distribution<double>(0.0, 1.0);
                    randGenerator = std::bind(distribution, generator);
                }

                double sample() const {
                    return randGenerator();
                }

                static Sampler1D *newInstance() {
                    return new Sampler1D();
                }

            private:
                std::uniform_real_distribution<double> distribution;
                std::mt19937 generator;
                std::function<double()> randGenerator;
            };
        }
    }
}

#endif //KAGUYA_SAMPLER_HPP
