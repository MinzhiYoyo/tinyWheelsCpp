//
// Created by 24983 on 25-2-22.
//

#ifndef DEQUE_DEF_H
#define DEQUE_DEF_H
#include "list.h"
namespace tinyWheels {

    struct DEQUE {
        using DEQUE_TYPE = unsigned char;
        // 低位 -> 高位，1表示使能，0表示失能
        // front_input -> front_output -> back_output -> back_input
        static constexpr DEQUE_TYPE FRONT_INPUT = 0x1;
        static constexpr DEQUE_TYPE FRONT_OUTPUT = 0x2;
        static constexpr DEQUE_TYPE BACK_OUTPUT = 0x4;
        static constexpr DEQUE_TYPE BACK_INPUT = 0x8;
    };

    // 使用list来实现的
    template<class T,DEQUE::DEQUE_TYPE MODE = 0xF>
    class deque {
        using container = list<T>;
        using Iterator = typename container::Iterator;
        container data;
    public:
        deque() = default;
        deque(const deque&) = default;
        deque(deque&&) = default;
        deque& operator=(const deque&) = default;
        deque& operator=(deque&&) = default;
        deque& operator=(const container& other) {
            data = other;
            return *this;
        }
        deque& operator=(container&& other) {
            data = std::move(other);
            return *this;
        }
        ~deque() = default;

        void push_front(const T& value) {
            if (MODE & 0x1) {
                data.push_front(value);
            }
        }
        void push_back(const T& value) {
            if (MODE & 0x8) {
                data.push_back(value);
            }
        }
        void pop_front() {
            if (MODE & 0x2) {
                data.pop_front();
            }
        }
        void pop_back() {
            if (MODE & 0x4) {
                data.pop_back();
            }
        }
        T& front() {
            return data.front();
        }
        T& back() {
            return data.back();
        }
        bool empty() {
            return data.empty();
        }
        size_t size() {
            return data.size();
        }

        Iterator begin() {return data.begin();}
        Iterator end() {return data.end();}
        Iterator rbegin() {return data.rbegin();}
        Iterator rend() {return data.rend();}
        Iterator cbegin() {return data.cbegin();}
        Iterator cend() {return data.cend();}
        Iterator crbegin() {return data.crbegin();}
        Iterator crend() {return data.crend();}

        friend void swap(deque& lhs, deque& rhs) noexcept{
            tinyWheels::swap(lhs.data, rhs.data);
        }
        void swap(deque& other) noexcept{
            swap(*this, other);
        }
    };
}
#endif //DEQUE_DEF_H
