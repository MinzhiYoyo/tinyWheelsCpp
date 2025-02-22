//
// Created by 24983 on 25-2-22.
//

#ifndef UTILITY_H
#define UTILITY_H
#include <utility>
namespace tinyWheels{
    template<class T>
    void swap(T &a, T &b) noexcept {
        T tmp = std::move(a);
        a = std::move(b);
        b = std::move(tmp);
    }
}

#endif //UTILITY_H
