//
// Created by 24983 on 25-2-22.
//

#ifndef QUEUE_H
#define QUEUE_H
#include "deque.h"
namespace tinyWheels {
    template<class T>
    class queue {
        using container = deque<T, DEQUE::FRONT_INPUT | DEQUE::BACK_OUTPUT>;
        using Iterator = typename container::Iterator;
        container data;
    public:
        void push(const T& value) {
            data.push_front(value);
        }
        void pop() {
            data.pop_back();
        }
        T& front() {
            return data.front();
        }
        T& back() {
            return data.back();
        }

    };
}
#endif //QUEUE_H
