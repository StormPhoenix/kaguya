//
// Created by Storm Phoenix on 2021/5/26.
//

#ifndef KAGUYA_TAGGEDPOINTER_H
#define KAGUYA_TAGGEDPOINTER_H

#include <kaguya/common.h>
#include <type_traits>

namespace RENDER_NAMESPACE {
    namespace memory {

        template<typename... Ts>
        struct TypePack {
            static const int count = sizeof...(Ts);
        };

        template<typename T, typename... Ts>
        struct TypeIndex {
            static const int index = 0;
            static_assert(!std::is_same<T, T>::value, "Type not in type parameter pack.");
        };

        template<typename T, typename... Ts>
        struct TypeIndex<T, TypePack<T, Ts...>> {
            static const int index = 0;
        };

        template<typename T, typename U, typename... Ts>
        struct TypeIndex<T, TypePack<U, Ts...>> {
            static const int index = TypeIndex<T, TypePack<Ts...>>::index + 1;
        };

        template<typename... Ts>
        class TaggedPointer {
        public:
            using Types = TypePack<Ts...>;

            template<typename T>
            RENDER_CPU_GPU TaggedPointer(T *ptr) {
                uintptr_t p = reinterpret_cast<uintptr_t>(ptr);
                ASSERT(p & ptrMask == p, "TaggedPointer address too large. ");
                constexpr unsigned int typeId = typeIndex<T>();
                taggedPtr = p | ((uintptr_t) typeId << typeShift);
            }

            template<typename T>
            RENDER_CPU_GPU int typeIndex() {
                using Tp = typename std::remove_cv_t<T>;
                if (std::is_same<Tp, std::nullptr_t>::value) {
                    // nullptr
                    return 0;
                } else {
                    return 1 + TypeIndex<T, Types>::index;
                }
            }

            RENDER_CPU_GPU unsigned int typeId() {
                return ((taggedPtr & typeMask) >> typeShift);
            }

            RENDER_CPU_GPU void *ptr() {
                return reinterpret_cast<void *>(taggedPtr & ptrMask);
            }

            template<typename T>
            RENDER_CPU_GPU bool isType() {
                return typeId() == typeIndex<T>();
            }

            template<typename T>
            RENDER_CPU_GPU T *cast() {
                ASSERT(isType<T>(), "TaggedPointer type can not be cast. ");
                return reinterpret_cast<T>(ptr());
            }

        private:
            static constexpr int typeBits = 7;
            static constexpr int typeShift = 64 - typeBits;
            static constexpr uint64_t typeMask = ((1ull << typeBits) - 1) << typeShift;
            static constexpr uint64_t ptrMask = ~typeMask;
            uintptr_t taggedPtr = 0;
        };
    }
}

#endif //KAGUYA_TAGGEDPOINTER_H
