//
// Created by Storm Phoenix on 2020/11/3.
//

#ifndef KAGUYA_SCOPESWAPPER_HPP
#define KAGUYA_SCOPESWAPPER_HPP

namespace kaguya {
    namespace memory {

        /**
         * 临时保存类型 T 的变量值，ScopeSwapper 被销毁时会恢复 target
         * @tparam T
         */
        template<typename T>
        class ScopeSwapper {
        public:
            ScopeSwapper(T *target = nullptr, T value = T()) : _target(target) {
                if (_target != nullptr) {
                    _backup = *_target;
                    *_target = value;
                }
            }

            ~ScopeSwapper() {
                if (_target != nullptr) {
                    *_target = _backup;
                }
            }

            ScopeSwapper(const ScopeSwapper &) = delete;

            ScopeSwapper &operator=(const ScopeSwapper &) = delete;

            /**
             * 右值引用，参考：https://blog.csdn.net/zhangsj1007/article/details/79940370
             * @param swapper
             * @return
             */
            ScopeSwapper &operator=(ScopeSwapper &&swapper) {
                if (_target != nullptr) {
                    *_target = _backup;
                }
                _target = swapper._target;
                _backup = swapper._backup;
                swapper._target = nullptr;
                return *this;
            }

        private:
            T *_target = nullptr;
            T _backup;
        };

    }
}

#endif //KAGUYA_SCOPESWAPPER_HPP
