//
// Created by 24983 on 25-2-22.
//

#ifndef TRAITS_H
#define TRAITS_H
#include <ostream>
#include <type_traits>

namespace tinyWheels{
    // 通过 SFINAE 检测是否可以使用 std::ostream << 进行输出
    template <typename T, typename = void>
    struct is_ostream_writable : std::false_type {};
    // 特化版本：如果 T 能够使用 std::ostream << 输出，则匹配此特化
    template <typename T>
    struct is_ostream_writable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> : std::true_type {};
    // 帮助函数，简化调用
    template <typename T>
    constexpr bool is_ostream_writable_v = is_ostream_writable<T>::value;  // 判断是否可以使用 std::ostream << 输出

}

#endif //TRAITS_H
