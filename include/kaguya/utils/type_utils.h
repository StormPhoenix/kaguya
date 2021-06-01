//
// Created by Graphics on 2021/5/28.
//

#ifndef KAGUYA_TYPE_UTILS_H
#define KAGUYA_TYPE_UTILS_H

#include <type_traits>

namespace RENDER_NAMESPACE {
    namespace type_utils {
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

        template<typename T>
        struct RemoveFirstType {
            using type = TypePack<T>;
            //    static_assert(!std::is_same<T, T>::value, "No parameter can be removed. ");
        };

        template<typename T, typename... Ts>
        struct RemoveFirstType<TypePack<T, Ts...>> {
            using type = TypePack<Ts...>;
        };

        template<typename T>
        struct GetFirstType {
        };

        template<typename T, typename... Ts>
        struct GetFirstType<TypePack<T, Ts...>> {
            using type = T;
        };

        template<int n>
        struct EvaluateTpType;

        template<>
        struct EvaluateTpType<1> {
            template<typename F, typename Tp, typename... Ts>
            inline auto operator()(F func, Tp tp, int index, TypePack<Ts...> types) {
                static_assert(sizeof...(Ts) >= 1, "Types can be zero. ");
                using T = typename GetFirstType<TypePack<Ts...>>::type;
                return func(tp.template cast<T>());
            }
        };

        template<int n>
        struct EvaluateTpType {
            template<typename F, typename TP, typename... Ts>
            inline auto operator()(F func, TP tp, int index, TypePack<Ts...> types) {
                if (index > 1) {
                    using RestType = typename RemoveFirstType<TypePack<Ts...>>::type;
                    return EvaluateTpType<n - 1>()(func, tp, index - 1, RestType());
                } else {
                    return EvaluateTpType<1>()(func, tp, index, types);
                }
            }
        };
    }
}

#endif //KAGUYA_TYPE_UTILS_H
