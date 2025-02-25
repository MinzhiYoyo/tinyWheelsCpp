//
// Created by 24983 on 25-2-22.
//

#ifndef REVERSE_ITERATOR_H
#define REVERSE_ITERATOR_H

#include "iterator.h"
#include "utility.h"

namespace tinyWheels{
    // 反向迭代器，用于连续存储的容器
    template<class Iterator>
    class ReverseIterator
    {
    public:
        // 定义所有的类型
        using iterator_type = Iterator; //  迭代器类型，连续容器一般是T*
        using iterator_category = typename iterator_traits<iterator_type>::iterator_category;
        using value_type = typename iterator_traits<iterator_type>::value_type;
        using difference_type = typename iterator_traits<iterator_type>::difference_type;
        using pointer = typename iterator_traits<iterator_type>::pointer;
        using reference = typename iterator_traits<iterator_type>::reference;
        using const_point = const pointer;
        using const_reference = const reference;
    private:
        Iterator cur_;  // 当前迭代器
    public:
        ReverseIterator() = default;
        explicit ReverseIterator(const Iterator& it) : cur_(it) {}
        ReverseIterator(const ReverseIterator& another) = default;
        ReverseIterator(ReverseIterator&& another) noexcept = default;
        ~ReverseIterator() = default;

        ReverseIterator& operator++() { // ++it
            --cur_;
            return *this;
        }
        ReverseIterator operator++(int) { // it++
            auto tmp = *this;
            ++(*this);
            return tmp;
        }
        ReverseIterator& operator--() { // --it
            ++cur_;
            return *this;
        }
        ReverseIterator operator--(int) { // it--
            auto tmp = *this;
            --(*this);
            return tmp;
        }
        ReverseIterator& operator+=(difference_type n) { // it += n
            cur_ -= n;
            return *this;
        }
        ReverseIterator& operator-=(difference_type n) { // it -= n
            cur_ += n;
            return *this;
        }

        friend ReverseIterator operator+(const ReverseIterator& it, difference_type n) { // it + n
            ReverseIterator tmp = it;
            tmp.cur_ -= n;
            return tmp;
        }
        friend ReverseIterator operator+(difference_type n, const ReverseIterator& it) { // n + it
            return it + n;
        }
        ReverseIterator operator-(difference_type n) { // it - n
            ReverseIterator tmp = *this;
            tmp.cur_ += n;
            return tmp;
        }
        difference_type operator-(const ReverseIterator& another) { // *this - another
            return another.cur_ - cur_;
        }

        // *this == another
        bool operator==(const ReverseIterator &another) const {
            return cur_ == another.cur_;
        }
        // *this != another
        bool operator!=(const ReverseIterator &another) const {
            return !(*this == another);
        }

        reference operator*() { // *it
            return *cur_;
        }
        pointer operator->() { // it->member
            return &(operator*());
        }

        // *this < another
        bool operator<(const ReverseIterator& another) const {
            return cur_ > another.cur_;
        }
        // *this <= another
        bool operator<=(const ReverseIterator& another) const {
            return cur_ >= another.cur_;
        }
        // *this > another
        bool operator>(const ReverseIterator& another) const {
            return cur_ < another.cur_;
        }
        // *this >= another
        bool operator>=(const ReverseIterator& another) const {
            return cur_ <= another.cur_;
        }

        // 赋值运算符
        ReverseIterator& operator=(const ReverseIterator& another) = default;
        ReverseIterator& operator=(const Iterator& it) {
            if (cur_ != it) {
                cur_ = it;
            }
            return *this;
        }
        ReverseIterator& operator=(ReverseIterator&& another) noexcept = default;
        ReverseIterator& operator=(Iterator&& it) {
            if (cur_ != it) {
                cur_ = it;
            }
            return *this;
        }
        friend void swap(ReverseIterator& a, ReverseIterator& b) noexcept {
            tinyWheels::swap(a.cur_, b.cur_);
        }
        void swap(ReverseIterator& a) noexcept {
            tinyWheels::swap(*this, a);
        }
    };
}

#endif //REVERSE_ITERATOR_H
