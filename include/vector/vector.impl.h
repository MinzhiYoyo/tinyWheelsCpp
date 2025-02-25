//
// Created by 24983 on 25-2-22.
//

#ifndef VECTOR_IMPL_H
#define VECTOR_IMPL_H

#include <bits/ranges_algobase.h>

#include "vector.def.h"
#include "algorithm.h"

namespace tinyWheels {

    template<class T, class Alloc>
    void vector<T, Alloc>::allocateAndFill(length_type n, const T &value) {
        auto [ptr, cap] = dataAllocator::allocate(n);
        data_ = ptr;
        capacity_ = cap;
        size_ = n;
        dataAllocator::construct(data_, n, std::forward<T>(value));
    }

    template<class T, class Alloc>
    void vector<T, Alloc>::allocateAndFill(length_type n, T &&value) {
        auto [ptr, cap] = dataAllocator::allocate(n);
        data_ = ptr;
        capacity_ = cap;
        size_ = n;
        dataAllocator::construct(data_, n, std::forward<T>(value));
    }

    template<class T, class Alloc>
    void vector<T, Alloc>::allocateAndFill(length_type n) {
        auto [ptr, cap] = dataAllocator::allocate(n);
        data_ = ptr;
        capacity_ = cap;
        size_ = n;
        dataAllocator::construct(data_, n);
    }

    template<class T, class Alloc>
    template<class InputIterator>
    void vector<T, Alloc>::move_data(InputIterator first, InputIterator last, InputIterator dst) {
        // 向前移动，先移动 first, last最好
        for (auto it = first, it2 = dst; it != last; ++it, ++it2) {
            *it2 = std::move(*it);
        }
    }

    template<class T, class Alloc>
    void vector<T, Alloc>::resize(length_type s) {
        this->recapacity(s, begin());
        size_ = s;
    }

    // 默认构造函数，也必须初始化空间，但是不初始化元素
    template<class T, class Alloc>
    vector<T, Alloc>::vector() {
        allocateAndFill(0);
    }

    // 拷贝构造函数
    template<class T, class Alloc>
    vector<T, Alloc>::vector(const vector &vec) {
        this->copy_from(std::forward<vector>(vec));
    }

    // 移动构造函数
    template<class T, class Alloc>
    vector<T, Alloc>::vector(vector &&vec) noexcept {
        this->move_from(std::forward<vector>(vec));
    }

    template<class T, class Alloc>
    vector<T, Alloc>::vector(length_type n) {
        allocateAndFill(n);
    }

    template<class T, class Alloc>
    vector<T, Alloc>::vector(length_type n, const T &value) {
        allocateAndFill(n, std::forward<T>(value));
    }

    template<class T, class Alloc>
    vector<T, Alloc>::vector(length_type n, T &&value) {
        allocateAndFill(n, std::forward<T>(value));
    }

    template<class T, class Alloc>
    vector<T, Alloc>::vector(const std::initializer_list<T> &il):vector(il.begin(), il.end()) {
    }

    template<class T, class Alloc>
    vector<T, Alloc>::vector(std::initializer_list<T> &&il):vector(il.begin(), il.end()) {
    }



    template<class T, class Alloc>
    template<class InputIterator>
    vector<T, Alloc>::vector(InputIterator first, InputIterator last) {
        if constexpr (std::is_integral_v<InputIterator>) {
            // 调用 vector(length_type n, T &&value) 构造函数
            this->allocateAndFill(first, std::forward<InputIterator>(last));
        }else {
            auto length = last - first;
            this->allocateAndFill(length);
            for (length_type i = 0; i < length; ++i) {
                data_[i] = *first++;
            }
        }
    }


    template<class T, class Alloc>
    vector<T, Alloc>::~vector() {
        if (data_ != nullptr) {
            dataAllocator::Destruct(data_, size_);
            dataAllocator::deallocate(data_, size_);
            data_ = nullptr;
        }
    }

    // 拷贝赋值运算符
    template<class T, class Alloc>
    vector<T, Alloc> &vector<T, Alloc>::operator=(const vector &vec) {
        if (this == &vec) {
            return *this;
        }
        ~vector();
        this->copy_from(std::forward<vector>(vec));
        return *this;
    }

    // 移动赋值运算符
    template<class T, class Alloc>
    vector<T, Alloc> &vector<T, Alloc>::operator=(vector &&vec) noexcept {
        if (this == &vec) {
            return *this;
        }
        ~vector();
        this->move_from(std::forward<vector>(vec));
        return *this;
    }

    template<class T, class Alloc>
    vector<T, Alloc> &vector<T, Alloc>::operator=(const std::initializer_list<T> &il) {
        ~vector();
        *this = vector(il);
        return *this;
    }

    template<class T, class Alloc>
    vector<T, Alloc> &vector<T, Alloc>::operator=(std::initializer_list<T> &&il) {
        ~vector();
        *this = vector(il);
        return *this;
    }

    template<class T, class Alloc>
    vector<T, Alloc> &vector<T, Alloc>::move_from(vector &&vec) noexcept {
        if (this != &vec) {
            data_ = vec.data_;
            capacity_ = vec.capacity_;
            size_ = vec.size_;
            vec.data_ = nullptr;
            vec.capacity_ = 0;
            vec.size_ = 0;
        }
        return *this;
    }

    template<class T, class Alloc>
    vector<T, Alloc> &vector<T, Alloc>::copy_from(const vector &vec) {
        if (this != &vec) {
            ~vector();
            auto [ptr, cap] = dataAllocator::allocate(vec.size());
            data_ = ptr;
            capacity_ = cap;
            size_ = vec.size();
            for (length_type i = 0; i < size_; ++i) {
                data_[i] = vec.data_[i];
            }
        }
        return *this;
    }

    template<class T, class Alloc>
    bool vector<T, Alloc>::operator==(const vector &vec) const {
        if (this == &vec) {
            return true;
        }
        if (size_ != vec.size_) {
            return false;
        }
        for (length_type i = 0; i < size_; ++i) {
            if (data_[i] != vec.data_[i]) {
                return false;
            }
        }
        return true;
    }

    template<class T, class Alloc>
    bool vector<T, Alloc>::operator!=(const vector &vec) const {
        return !(*this == vec);
    }

    template<class T, class Alloc>
    void vector<T, Alloc>::push_back(const T &value) {
        insert(end(), value);
    }

    template<class T, class Alloc>
    void vector<T, Alloc>::push_back(T &&value) {
        insert(end(), value);
    }

    template<class T, class Alloc>
    template<class... Args>
    void vector<T, Alloc>::emplace_back(Args &&... args) {
        insert(end(),T(std::forward<Args>(args)...));
    }

    template<class T, class Alloc>
    bool vector<T, Alloc>::pop_back() {
        if (size_ == 0) {
            return false;
        }
        dataAllocator::Destruct(data_ + size_ - 1, 1);
        --size_;
        return true;
    }

    template<class T, class Alloc>
    template<class InputIterator>
    InputIterator vector<T, Alloc>::recapacity(length_type new_size, InputIterator it) {
        new_size = new_size == -1 ? size_ + 1 : new_size;
        if (new_size > capacity_) {  // 只有大于容量时才需要迁移数据
            auto difference = it - begin();

            auto [ptr, cap] = dataAllocator::allocate(new_size);
            auto it1 = ptr;
            auto it2 = data_;
            for (length_type i = 0; i < size_; ++i) {
                *it1++ = std::move(*it2++);
            }
            dataAllocator::deallocate(data_, size_);  // 只需要释放内存，不要执行构造函数，因为对象仍然存在，对象只是搬家了而已
            data_ = ptr;
            capacity_ = cap;
            it = begin() + difference;
        }
        return it;
    }

    template<class T, class Alloc>
    template<class InputIterator>
    void vector<T, Alloc>::insert(InputIterator it, const T &value) {
        insert(it, 1, value);
    }

    template<class T, class Alloc>
    template<class InputIterator>
    void vector<T, Alloc>::insert(InputIterator it, T &&value) {
        insert(it, 1, value);
    }

    template<class T, class Alloc>
    template<class InputIterator>
    void vector<T, Alloc>::insert(InputIterator it, length_type n, const T &value) {
        it = recapacity(size_ + n, it);
        reverseIterator last(it);
        last += 1;
        reverseIterator dst (it + n + size_);
        move_data(rbegin(), last, dst);  // 把 [it, end()) 之前的数据向后移动 n 个位置
        fill(it, it + n, value);
        size_ = size_ + n;
    }

    template<class T, class Alloc>
    template<class InputIterator>
    void vector<T, Alloc>::insert(InputIterator it, length_type n, T &&value) {
        it = recapacity(size_ + n, it);
        reverseIterator last(it);
        last += 1;
        reverseIterator dst(it + n + size_);
        move_data(rbegin(), last, dst);  // 把 [it, end()) 之前的数据向后移动 n 个位置
        fill(it, it + n, std::forward<T>(value));
        size_ = size_ + n;
    }

    template<class T, class Alloc>
    template<class InputIterator, class InputIterator2>
    requires(not std::is_integral_v<InputIterator2>)
    void vector<T, Alloc>::insert(InputIterator it, InputIterator2 first, InputIterator2 last) {
        auto n = last - first;
        it = recapacity(size_ + n, it);
        reverseIterator rlast(it);
        rlast += 1;
        reverseIterator dst(it + n + size_);
        move_data(rbegin(), rlast, dst);  // 把 [it, end()) 之前的数据向后移动 n 个位置
        for (auto it1 = first, it2 = it; it1 != last; ++it1, ++it2) {
            *it2 = *it1;
        }
        size_ = size_ + n;
    }

    template<class T, class Alloc>
    template<class InputIterator>
    void vector<T, Alloc>::erase(InputIterator it) {
        erase(it, it + 1);
    }

    template<class T, class Alloc>
    template<class InputIterator>
    void vector<T, Alloc>::erase(InputIterator first, InputIterator last) {
        // auto start_erase = get_index_by_iterator(first);
        // auto start_left = get_index_by_iterator(last);
        dataAllocator::Destruct(first, last - first);
        move_data(last, end(), first);
        size_ -= last - first;
    }

}

#endif //VECTOR_IMPL_H
