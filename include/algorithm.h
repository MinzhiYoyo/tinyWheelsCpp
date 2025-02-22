//
// Created by 24983 on 25-2-22.
//

#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <cstring>

namespace tinyWheels{
    template<class ForwardIterator, class T>
    void fill(ForwardIterator first, ForwardIterator last, const T& value) {
        for (; first != last; ++first) {
            *first = value;
        }
    }
    inline void fill(char *first, char *last, const char& value) {
        memset(first, value, last - first);
    }

    // fill_n 函数
    template<class OutputIterator, class Size, class T>
    OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
        for (; n > 0; --n, ++first) {
            *first = value;
        }
        return first;
    }
    inline char* fill_n(char *first, size_t n, const char& value) {
        memset(first, value, n);
        return first + n;
    }


    // abs函数
    template<class T>
    T abs(const T& value) {
        return value < 0 ? -value : value;
    }
}

#endif //ALGORITHM_H
