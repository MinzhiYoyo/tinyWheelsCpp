//
// Created by 24983 on 25-2-22.
//

#ifndef STACK_H
#define STACK_H
#include "vector.h"

namespace tinyWheels {

    template<class T>
    class stack {
        using Container = vector<T>;

        Container container;
        // stack& copy_from(const stack& st);
        // stack& move_from(stack&& st) noexcept;
    public:
        stack() = default;
        stack(const stack& st):stack() {
            copy_from(st);
        }
        stack(stack&& st) noexcept:stack() {
            move_from(std::forward<stack>(st));
        }
        explicit stack(const Container& c) {
            container.copy_from(c);
        }
        explicit stack(Container&& c) {
            container.move_from(c);
        }
        ~stack() = default;

        void push(const T& value) {
            container.push_back(value);
        }
        void push(T&& value) {
            container.push_back(std::forward<T>(value));
        }

        bool pop() {
            if (empty()) return false;
            container.pop_back();
            return true;
        }
        T& top() {
            return container.back();
        }

        typename Container::length_type size() {return container.size();}
        bool empty() {return container.empty();}
        friend void swap(stack& s1, stack& s2) noexcept {
            tinyWheels::swap(s1.container, s2.container);
        }
        void swap(stack& s) noexcept {
            swap(*this, s);
        }
    };
}


#endif //STACK_H
