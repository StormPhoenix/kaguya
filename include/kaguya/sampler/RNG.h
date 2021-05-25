//
// Created by Storm Phoenix on 2021/5/25.
//
// Reference: https://github.com/wjakob/pcg32
//

#ifndef KAGUYA_RNG_H
#define KAGUYA_RNG_H

#define PCG32_DEFAULT_STATE  0x853c49e6748fea9bULL
#define PCG32_DEFAULT_STREAM 0xda3e39cb94b95bdbULL
#define PCG32_MULT           0x5851f42d4c957f2dULL

#include <inttypes.h>
#include <kaguya/common.h>
#include <kaguya/math/Math.h>

namespace RENDER_NAMESPACE {
    namespace sampler {

        using math::ONE_MINUS_EPSILON;

        class RNG {
        public:
            RENDER_CPU_GPU
            RNG() : state(PCG32_DEFAULT_STATE), inc(PCG32_DEFAULT_STREAM) {};

            RENDER_CPU_GPU
            RNG(uint64_t initState, uint64_t initSeq) { newSequence(initState, initSeq); }

            template<typename T>
            RENDER_CPU_GPU T uniform();

            RENDER_CPU_GPU
            void newSequence(uint64_t initState, uint64_t initSeq = 1);

            RENDER_CPU_GPU
            void advance(int64_t delta);

        private:
            uint64_t state;
            uint64_t inc;
        };

        template<>
        inline uint32_t RNG::uniform<uint32_t>() {
            uint64_t oldstate = state;
            state = oldstate * PCG32_MULT + inc;
            uint32_t xorshifted = (uint32_t) (((oldstate >> 18u) ^ oldstate) >> 27u);
            uint32_t rot = (uint32_t) (oldstate >> 59u);
            return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
        }

        template<>
        inline uint64_t RNG::uniform<uint64_t>() {
            uint64_t v0 = uniform<uint32_t>(), v1 = uniform<uint32_t>();
            return (v0 << 32) | v1;
        }

        // https://stackoverflow.com/a/13208789
        template<>
        inline int32_t RNG::uniform<int32_t>() {
            // https://stackoverflow.com/a/13208789
            uint32_t v = uniform<uint32_t>();
            if (v <= (uint32_t) std::numeric_limits<int32_t>::max()) {
                return int32_t(v);
            }
            return int32_t(v - std::numeric_limits<int32_t>::min()) +
                   std::numeric_limits<int32_t>::min();
        }

        // https://stackoverflow.com/a/13208789
        template<>
        inline int64_t RNG::uniform<int64_t>() {
            uint64_t v = uniform<uint64_t>();
            if (v <= (uint64_t) std::numeric_limits<int64_t>::max()) {
                // Safe to type convert directly.
                return int64_t(v);
            }
            return int64_t(v - std::numeric_limits<int64_t>::min()) +
                   std::numeric_limits<int64_t>::min();
        }

        template<>
        inline float RNG::uniform<float>() {
            return std::min<float>(ONE_MINUS_EPSILON, uniform<uint32_t>() * 0x1p-32f);
        }

        template<>
        inline double RNG::uniform<double>() {
            return std::min<double>(ONE_MINUS_EPSILON, uniform<uint64_t>() * 0x1p-64);
        }

        inline void RNG::newSequence(uint64_t initState, uint64_t initSeq) {
            state = 0u;
            inc = (initSeq << 1u) | 1u;
            uniform<uint32_t>();
            state += initState;
            uniform<uint32_t>();
        }

        /**
        * \brief Multi-step advance function (jump-ahead, jump-back)
        *
        * The method used here is based on Brown, "Random Number Generation
        * with Arbitrary Stride", Transactions of the American Nuclear
        * Society (Nov. 1994). The algorithm is very similar to fast
        * exponentiation.
        */
        inline void RNG::advance(int64_t delta) {
            uint64_t curMult = PCG32_MULT;
            uint64_t curPlus = inc;
            uint64_t accMult = 1u;
            uint64_t accPlus = 0u;

            /* Even though delta is an unsigned integer, we can pass a signed
               integer to go backwards, it just goes "the long way round". */
            uint64_t _delta = (uint64_t) delta;

            while (_delta > 0) {
                if (_delta & 1) {
                    accMult *= curMult;
                    accPlus = accPlus * curMult + curPlus;
                }
                curPlus = (curMult + 1) * curPlus;
                curMult *= curMult;
                _delta /= 2;
            }
            state = accMult * state + accPlus;
        }
    }
}
#endif //KAGUYA_RNG_H
