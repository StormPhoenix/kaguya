//
// Created by Storm Phoenix on 2021/4/9.
//

#ifndef KAGUYA_ATOMICFLOAT_H
#define KAGUYA_ATOMICFLOAT_H

#include <kaguya/math/Math.h>

namespace kaguya {
    namespace parallel {

        using namespace math;

        class AtomicFloat {
        public:
            AtomicFloat(Float val = 0) {
                bits = float2bits(val);
            }

            Float get() {
                return bits2float(bits);
            }

            Float operator=(Float val) {
                bits = float2bits(val);
                return val;
            }

            void add(Float val) {
#if defined(KAGUYA_DATA_DOUBLE)
                uint64_t oldBits = bits;
                uint64_t newBits;
#else
                uint32_t oldBits = bits;
                uint32_t newBits;
#endif
                do {
                    // TODO 具体看下为什么要这么做
                    newBits = float2bits(bits2float(oldBits) + val);
                } while (!bits.compare_exchange_weak(oldBits, newBits));
            }

        private:
#if defined(KAGUYA_DATA_DOUBLE)
            std::atomic<uint64_t> bits;
#else
            std::atomic<uint32_t> bits;
#endif
        };

    }
}
#endif //KAGUYA_ATOMICFLOAT_H
