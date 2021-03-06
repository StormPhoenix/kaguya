//
// Created by Storm Phoenix on 2020/10/14.
//

#ifndef KAGUYA_SPECTRUM_HPP
#define KAGUYA_SPECTRUM_HPP

#include <stddef.h>
#include <cmath>
#include <cassert>
#include <algorithm>

#include <kaguya/math/Math.h>

namespace kaguya {
    namespace core {

        /**
         * 光谱基类
         * @tparam SpectrumSample
         */
        template<int SpectrumSamples>
        class SpectrumTemplate {
        public:
            SpectrumTemplate(Float v = 0.f) {
                for (int i = 0; i < SpectrumSamples; ++i) {
                    value[i] = v;
                }
                assert(!hasNans());
            }

            SpectrumTemplate(const SpectrumTemplate &s) {
                for (int i = 0; i < SpectrumSamples; ++i) value[i] = s.value[i];
            }

            SpectrumTemplate clamp(Float low = 0, Float high = math::infinity) const {
                SpectrumTemplate ret;
                for (int i = 0; i < SpectrumSamples; ++i)
                    ret.value[i] = math::clamp(value[i], low, high);
                DCHECK(!ret.HasNaNs());
                return ret;
            }

            SpectrumTemplate &operator=(const SpectrumTemplate &s) {
                assert(!s.hasNans());
                for (int i = 0; i < SpectrumSamples; ++i) value[i] = s.value[i];
                return *this;
            }


            SpectrumTemplate &operator+=(const SpectrumTemplate &s2) {
                assert(!s2.hasNans());
                for (int i = 0; i < SpectrumSamples; ++i) value[i] += s2.value[i];
                return *this;
            }

            SpectrumTemplate operator+(const SpectrumTemplate &s2) const {
                assert(!s2.hasNans());
                SpectrumTemplate ret = *this;
                for (int i = 0; i < SpectrumSamples; ++i) ret.value[i] += s2.value[i];
                return ret;
            }

            SpectrumTemplate operator-(const SpectrumTemplate &s2) const {
                assert(!s2.hasNans());
                SpectrumTemplate ret = *this;
                for (int i = 0; i < SpectrumSamples; ++i) ret.value[i] -= s2.value[i];
                return ret;
            }

            SpectrumTemplate operator/(const SpectrumTemplate &s2) const {
                assert(!s2.hasNans());
                SpectrumTemplate ret = *this;
                for (int i = 0; i < SpectrumSamples; ++i) {
                    assert(s2.value[i] != 0);
                    ret.value[i] /= s2.value[i];
                }
                return ret;
            }

            SpectrumTemplate operator*(const SpectrumTemplate &sp) const {
                assert(!sp.hasNans());
                SpectrumTemplate ret = *this;
                for (int i = 0; i < SpectrumSamples; ++i) ret.value[i] *= sp.value[i];
                return ret;
            }

            SpectrumTemplate &operator*=(const SpectrumTemplate &sp) {
                assert(!sp.hasNans());
                for (int i = 0; i < SpectrumSamples; ++i) value[i] *= sp.value[i];
                return *this;
            }

            SpectrumTemplate operator*(Float a) const {
                SpectrumTemplate ret = *this;
                for (int i = 0; i < SpectrumSamples; ++i) ret.value[i] *= a;
                assert(!ret.hasNans());
                return ret;
            }

            SpectrumTemplate &operator*=(Float a) {
                for (int i = 0; i < SpectrumSamples; ++i) value[i] *= a;
                assert(!hasNans());
                return *this;
            }

            friend inline SpectrumTemplate operator*(Float a, const SpectrumTemplate &s) {
                assert(!std::isnan(a) && !s.hasNans());
                return s * a;
            }

            SpectrumTemplate operator/(Float a) const {
                assert(a != 0);
                assert(!std::isnan(a));
                SpectrumTemplate ret = *this;
                for (int i = 0; i < SpectrumSamples; ++i) ret.value[i] /= a;
                assert(!ret.hasNans());
                return ret;
            }

            SpectrumTemplate &operator/=(Float a) {
                assert(a != 0);
                assert(!std::isnan(a));
                for (int i = 0; i < SpectrumSamples; ++i) value[i] /= a;
                return *this;
            }

            bool operator==(const SpectrumTemplate &sp) const {
                for (int i = 0; i < SpectrumSamples; ++i)
                    if (value[i] != sp.value[i]) return false;
                return true;
            }

            bool operator!=(const SpectrumTemplate &sp) const {
                return !(*this == sp);
            }

            bool isBlack() const {
                for (int i = 0; i < SpectrumSamples; ++i)
                    if (value[i] != 0.) return false;
                return true;
            }

            friend SpectrumTemplate sqrt(const SpectrumTemplate &s) {
                SpectrumTemplate ret;
                for (int i = 0; i < SpectrumSamples; ++i) ret.value[i] = std::sqrt(s.value[i]);
                assert(!ret.hasNans());
                return ret;
            }

            SpectrumTemplate operator-() const {
                SpectrumTemplate ret;
                for (int i = 0; i < SpectrumSamples; ++i) ret.value[i] = -value[i];
                return ret;
            }

            friend SpectrumTemplate exp(const SpectrumTemplate &s) {
                SpectrumTemplate ret;
                for (int i = 0; i < SpectrumSamples; ++i) ret.value[i] = std::exp(s.value[i]);
                assert(!ret.hasNans());
                return ret;
            }

            SpectrumTemplate truncate(Float low = 0, Float high = math::infinity) const {
                SpectrumTemplate ret;
                for (int i = 0; i < SpectrumSamples; ++i)
                    ret.value[i] = math::clamp(value[i], low, high);
                assert(!ret.hasNans());
                return ret;
            }

            Float maxValue() const {
                Float m = value[0];
                for (int i = 1; i < SpectrumSamples; ++i)
                    m = std::max(m, value[i]);
                return m;
            }

            bool hasNans() const {
                for (int i = 0; i < SpectrumSamples; ++i)
                    if (std::isnan(value[i])) return true;
                return false;
            }

            Float &operator[](int i) {
                assert(i >= 0 && i < SpectrumSamples);
                return value[i];
            }

            Float operator[](int i) const {
                assert(i >= 0 && i < SpectrumSamples);
                return value[i];
            }

        protected:
            Float value[SpectrumSamples];
        };

        class RGBSpectrum : public SpectrumTemplate<3> {
            using SpectrumTemplate<3>::value;
        public:
            RGBSpectrum(Float value = 0.0f) : SpectrumTemplate<3>(value) {}

            RGBSpectrum(const SpectrumTemplate<3> &spectrum) : SpectrumTemplate<3>(spectrum) {}

            Float r() const { return value[0]; }

            void r(Float r) { value[0] = r; }

            Float g() const { return value[1]; }

            void g(Float g) { value[1] = g; }

            Float b() const { return value[2]; }

            void b(Float b) { value[2] = b; }
        };
    }
}

#endif //KAGUYA_SPECTRUM_HPP
