//
// Created by 24983 on 25-2-22.
//

#ifndef LIST_IMPL_H
#define LIST_IMPL_H
#include "list.def.h"


namespace tinyWheels {

    template<class T>
    void list<T>::allocateAndFill(length_type n) {
        auto [ptr, cap] = nodeAllocator::allocate(n);
        auto prev_address = head_;
        Iterator next_address(nullptr);
        for (auto i = 0; i < n; ++i) {
            next_address = i < n - 1 ? Iterator(ptr+i+1): tail_;
            nodeAllocator::construct(ptr + i, 1, T(), prev_address.get(), next_address.get());
            prev_address = ptr + i;
        }
        head_.get()->next(ptr);
        tail_.get()->prev(ptr + n - 1);
        size_ = n;
    }

    template<class T>
    void list<T>::allocateAndFill(length_type n, const T &value) {
        auto [ptr, cap] = nodeAllocator::allocate(n);
        auto prev_address = head_;
        Iterator next_address(nullptr);
        for (auto i = 0; i < n; ++i) {
            next_address = i < n - 1 ? Iterator(ptr+i+1): tail_;
            nodeAllocator::construct(ptr + i, 1, value, prev_address.get(), next_address.get());
            prev_address = ptr + i;
        }
        head_.get()->next(ptr);
        tail_.get()->prev(ptr + n - 1);
        size_ = n;
    }
    template<class T>
    void list<T>::allocateAndFill(length_type n, T &&value) {
        auto [ptr, cap] = nodeAllocator::allocate(n);
        auto prev_address = head_;
        Iterator next_address(nullptr);
        for (auto i = 0; i < n; ++i) {
            next_address = i < n - 1 ? Iterator(ptr+i+1): tail_;
            nodeAllocator::construct(ptr + i, 1, std::forward<T>(value), prev_address.get(), next_address.get());
            prev_address = ptr + i;
        }
        head_.get()->next(ptr);
        tail_.get()->prev(ptr + n - 1);
        size_ = n;
    }


    template<class T>
    list<T>& list<T>::copy_from(const list &l) {
        if (this != &l) {
            ~list();
            auto size = l.size_;
            auto [start, cap] = nodeAllocator::allocate(size);
            auto it = l.begin();
            auto prev_address = head_;
            Iterator next_address(nullptr);
            for (auto i = 0; i < size; ++i, ++it) {
                next_address = i < size - 1 ? Iterator(start+i+1) : tail_;
                nodeAllocator::construct(start + i, 1, *it, prev_address.get(), next_address.get());
                prev_address = start + i;
            }
            head_.get()->next(start);
            tail_.get()->prev(start + size - 1);
            size_ = size;
        }
        return *this;
    }

    template<class T>
    list<T> &list<T>::move_from(list &&l) noexcept {
        if (this != &l) {
            ~list();
            *this = std::move(l);
        }
        return *this;
    }

    template<class T>
    list<T>::list() {  // 默认初始化
        auto[ptr, cap] = nodeAllocator::allocate(2);
        nodeAllocator::construct(ptr, 1, T(), nullptr, ptr + 1);
        nodeAllocator::construct(ptr + 1, 1, T(), ptr, nullptr);
        head_ = ptr;
        tail_ = ptr + 1;
    }

    template<class T>
    list<T>::list(const list &l):list() {
        copy_from(std::forward<list>(l));
    }

    template<class T>
    list<T>::list(list &&l) noexcept :list() {
        move_from(std::forward<list>(l));
    }

    template<class T>
    list<T>::list(const length_type n) :list(){
        allocateAndFill(n);
    }
    template<class T>
    list<T>::list(length_type n, const T &value) :list(){
        allocateAndFill(n, value);
    }
    template<class T>
    list<T>::list(length_type n, T &&value) :list(){
        allocateAndFill(n, value);
    }

    template<class T>
    template<class InputIterator>
    requires(not std::is_integral_v<InputIterator>)
    list<T>::list(InputIterator first, InputIterator last) :list(){
        if constexpr (std::is_integral_v<InputIterator>) {
            allocateAndFill(first, last);
        }else {
            auto it = first;
            auto prev_address = head_;
            while (it != last) {
                auto [ptr, cap] = nodeAllocator::allocate(1);
                if constexpr (std::is_same_v<InputIterator, Iterator>) {
                    nodeAllocator::construct(ptr, 1, (*it).data(), prev_address.get(), tail_.get());
                }else {
                    nodeAllocator::construct(ptr, 1, *it, prev_address.get(), tail_.get());
                }
                prev_address->next(ptr);  // 上一个地址的下一个地址是ptr
                prev_address = ptr;
                ++it;
                ++size_;
            }
            tail_.get()->prev(prev_address.get());
        }
    }

    template<class T>
    list<T>::list(const std::initializer_list<T> &il):list(il.begin(), il.end()) {
    }
    template<class T>
    list<T>::list(std::initializer_list<T> &&il):list(il.begin(), il.end()) {
    }

    template<class T>
    list<T>::~list() {
        auto it = begin();
        while (it != end()) {
            auto tmp = it;
            ++it;
            // 删除tmp并回收空间
            nodeAllocator::Destruct(tmp.get(), 1);
            nodeAllocator::deallocate(tmp.get(), 1);
        }
    }

    template<class T>
    list<T> &list<T>::operator=(const list &l) {
        if (this != &l) {
            copy_from(std::forward<list>(l));
        }
        return *this;
    }
    template<class T>
    list<T> &list<T>::operator=(list &&l) noexcept {
        if (this != &l) {
            move_from(std::forward<list>(l));
        }
        return *this;
    }

    template<class T>
    list<T> &list<T>::operator=(const std::initializer_list<T> &il) {
        ~list();
        *this = list(std::forward<std::initializer_list<T>>(il));
        return *this;
    }

    template<class T>
    list<T> &list<T>::operator=(std::initializer_list<T> &&il) {
        ~list();
        *this = list(std::forward<std::initializer_list<T>>(il));
        return *this;
    }

    template<class T>
    bool list<T>::operator==(const list &l) const {
        if (size_ != l.size_) return false;
        auto it1 = begin();
        auto it2 = l.begin();
        while (it1 != end()) {
            if (*it1 != *it2) return false;
            ++it1;
            ++it2;
        }
        return true;
    }

    template<class T>
    T &list<T>::operator[](length_type index) {
        if (index >= size_) throw exception("index out of range, index: %lu, size: %lu", index, size_);
        if (index < size_ / 2) {
            auto it = begin();
            while (index) {
                --index;
                ++it;
            }
            return *it;
        }
        index = size_ - index - 1;
        auto it = rbegin();
        while (index) {
            --index;
            ++it;
        }
        return *it;
    }

    // 插入与删除元素
    template<class T>
    void list<T>::push_back(const T& value) {
        insert(end(), std::forward<T>(value));
    }
    template<class T>
    void list<T>::push_back(T&& value) {
        insert(end(), std::forward<T>(value));
    }
    template<class T>
    void list<T>::push_front(const T &value) {
        insert(begin(), std::forward<T>(value));
    }
    template<class T>
    void list<T>::push_front(T &&value) {
        insert(begin(), std::forward<T>(value));
    }

    template<class T>
    bool list<T>::pop_back() {
        if (size_ == 0) return false;
        auto it = tail_.get()->prev();
        it.get()->prev()->next(it->next());
        it.get()->next()->prev(it->prev());
        nodeAllocator::Destruct(it, 1);
        nodeAllocator::deallocate(it, 1);
        --size_;
        return true;
    }
    template<class T>
    bool list<T>::pop_front() {
        if (size_ == 0) return false;
        auto it = head_->next();
        it.get()->prev()->next(it.get()->next());
        it.get()->next()->prev(it.get()->prev());
        nodeAllocator::Destruct(it, 1);
        nodeAllocator::deallocate(it, 1);
        --size_;
        return true;
    }

    template<class T>
    template<class InputIterator>
    void list<T>::insert(InputIterator it, const T &value) {
        insert(it, 1, std::forward<T>(value));
    }
    template<class T>
    template<class InputIterator>
    void list<T>::insert(InputIterator it, T &&value) {
        insert(it, 1, std::forward<T>(value));
    }
    template<class T>
    template<class InputIterator>
    void list<T>::insert(InputIterator it, length_type n, const T &value) {
        list l(n, std::forward<T>(value));
        merge(it, l.begin(), l.end(), n);
    }
    template<class T>
    template<class InputIterator>
    void list<T>::insert(InputIterator it, length_type n, T &&value) {
        list l(n, std::forward<T>(value));
        merge(it, l.begin(), l.end(), n);
    }

    template<class T>
    template<class InputIterator, class InputIterator2>
    requires(not std::is_integral_v<InputIterator>)
    void list<T>::insert(InputIterator it, InputIterator2 first, InputIterator2 last) {
        // if constexpr (std::is_integral_v<InputIterator2>) {
        //     insert(it, static_cast<length_type>(first), static_cast<InputIterator2>(last));
        // }else {
            list l(first, last);
            merge(it, l.begin(), l.end(), abs(last - first));
        // }
    }

    template<class T>
    template<class InputIterator>
    void list<T>::erase(InputIterator it) {
        erase(it, it+1);
    }

    // 删除 [first, last)
    template<class T>
    template<class InputIterator>
    void list<T>::erase(InputIterator first, InputIterator last) {
        // 提取 [first, last) 之间的节点到新链表，等l自然析构即可
        list l;
        auto length = abs(last - first);
        l.merge(l.begin(), first, last, length);  // 无所谓多长
        size_ -= length;
    }


    template<class T>
    template<class InputIterator, class InputIterator2>
    list<T>& list<T>::merge(InputIterator it, InputIterator2 first, InputIterator2 last, length_type n) {
        auto head = first.get();
        auto tail = (*last).prev();

        // 1. 脱离
        head->prev()->next(tail->next());
        tail->next()->prev(head->prev());

        // 2. 加入
        auto it_prev = (*it).prev();
        auto cur = it.get();

        it_prev->next(head);
        head->prev(it_prev);

        tail->next(cur);
        cur->prev(tail);
        size_ += n;
        return *this;
    }

}
#endif //LIST_IMPL_H
