//
// Created by 24983 on 25-2-22.
//

#ifndef LIST_DEF_H
#define LIST_DEF_H

#include <algorithm>
#include <ostream>

#include "iterator.h"
#include "allocator.h"
#include "traits.h"
#include "utility.h"

namespace tinyWheels {

    namespace mzList {
        template <class Iterator> class ListIterator;
        template <class Iterator> class ReverseListIterator;

        template <class T>
        class ListNode {
            T data_;
            ListNode*prev_ = nullptr;
            ListNode*next_ = nullptr;
        public:
            ListNode() = default;
            explicit ListNode(const T& data, ListNode* prev = nullptr, ListNode* next = nullptr)
                    : data_(data), prev_(prev), next_(next) {}
            ~ListNode() = default;
            const T& data() const {return data_;}
            void data(const T& data){data_ = data;}
            void data(T&& data) noexcept {data_ = std::move(data);}
            ListNode* prev() const {return prev_;}
            ListNode* next() const {return next_;}
            void next(ListNode* n){next_ = n;}
            void prev(ListNode* p){prev_ = p;}
            ListNode& operator=(const ListNode& node) {
                if (this != &node) {
                    data_ = node.data_;
                    prev_ = node.prev_;
                    next_ = node.next_;
                }
                return *this;
            }
            bool operator==(const ListNode& node) const {
                return data_ == node.data_;
            }
            bool operator!=(const ListNode& node) const {
                return !(*this == node);
            }
            bool operator==(const T& value) const {
                return data_ == value;
            }
            bool operator!=(const T& value) const {
                return !(*this == value);
            }
            bool equal_to(const ListNode& node) const {
                return data_ == node.data_ and prev_ == node.prev_ and next_ == node.next_;
            }
            ListNode& operator=(ListNode&& node) noexcept {
                if (this != &node) {
                    data_ = std::move(node.data_);
                    prev_ = std::move(node.prev_);
                    next_ = std::move(node.next_);
                }
                return *this;
            }
            ListNode& operator=(const T& value) {
                data_ = value;
                return *this;
            }
            ListNode& operator=(T&& value) {
                data_ = std::move(value);
                return *this;
            }
        };

        // 迭代器与反向迭代器
        template <class Iterator>
        class ListIterator {
            using iterator_type = Iterator; //  迭代器类型，连续容器一般是T*
            using iterator_category = typename iterator_traits<iterator_type>::iterator_category;
            using value_type = typename iterator_traits<iterator_type>::value_type;
            using difference_type = typename iterator_traits<iterator_type>::difference_type;
            using pointer = typename iterator_traits<iterator_type>::pointer;
            using reference = typename iterator_traits<iterator_type>::reference;
            using const_point = const pointer;
            using const_reference = const reference;

            Iterator cur_;
        public:
            Iterator get() const {return cur_;}
            ListIterator() = default;
            ListIterator(const ListIterator& it): cur_(it.cur_) {}
            explicit ListIterator(const Iterator& it): cur_(it) {}
            ListIterator(ListIterator&& it) noexcept: cur_(std::move(it.cur_)) {}
            explicit ListIterator(Iterator& it): cur_(it) {}
            ~ListIterator() = default;
            ListIterator& operator=(const ListIterator& it) = default;
            ListIterator& operator=(ListIterator&& it) noexcept = default;
            ListIterator& operator=(const Iterator& it) {cur_ = it; return *this;}
            ListIterator& operator=(Iterator&& it) {cur_ = it;return *this;}

            // 解引用
            reference operator*() { // *it
                return *cur_;
            }
            pointer operator->() { // it->member
                return &operator*();
            }

            ListIterator& operator++() {  // ++it
                cur_ = operator*().next();
                return *this;
            }
            ListIterator operator++(int) { // it++
                auto tmp = *this;
                ++(*this);
                return tmp;
            }
            ListIterator& operator--() { // --it
                cur_ = operator*().prev();
                return *this;
            }
            ListIterator operator--(int) { // it--
                auto tmp = *this;
                --(*this);
                return tmp;
            }

            ListIterator& operator+=(difference_type n) { // it += n
                while (n > 0) {
                    ++(*this);
                    --n;
                }
                return *this;
            }
            ListIterator& operator-=(difference_type n) { // it -= n
                while (n > 0) {
                    --(*this);
                    --n;
                }
                return *this;
            }

            friend ListIterator operator+(const ListIterator& it, difference_type n) { // it + n
                ListIterator tmp = it;
                tmp += n;
                return tmp;
            }
            friend ListIterator operator+(difference_type n, const ListIterator& it) { // n + it
                return it + n;
            }
            ListIterator operator-(difference_type n) { // it - n
                ListIterator tmp = *this;
                tmp -= n;
                return tmp;
            }
            difference_type operator-(const ListIterator& another) { // *this - another
                difference_type n = 0;
                auto tmp = another;
                while (tmp != *this and tmp.cur_ != nullptr) {
                    --tmp;
                    ++n;
                }
                if (tmp == *this) return n;
                tmp = *this;
                n = 0;
                while (tmp != another and tmp.cur_ != nullptr) {
                    --tmp;
                    --n;
                }
                return n;
            }

            bool operator==(const ListIterator &another) const {
                return cur_ == another.cur_;
            }
            bool operator!=(const ListIterator &another) const {
                return !(*this == another);
            }

            bool operator<(const ListIterator &another) const {
                auto it = *this;
                while (it != another and it != nullptr) {
                    ++it;
                }
                return it == another;
            }
            bool operator>(const ListIterator &another) const {
                return another < *this;
            }
            bool operator<=(const ListIterator &another) const {
                return !(*this > another);
            }
            bool operator>=(const ListIterator &another) const {
                return !(*this < another);
            }
            friend void swap(ListIterator& a, ListIterator& b) noexcept {
                swap(a.cur_, b.cur_);
            }
            void swap(ListIterator& a) noexcept {
                swap(*this, a);
            }
        };

        template <class Iterator>
        class ReverseListIterator {
            using iterator_type = Iterator; //  迭代器类型，连续容器一般是T*
            using iterator_category = typename iterator_traits<iterator_type>::iterator_category;
            using value_type = typename iterator_traits<iterator_type>::value_type;
            using difference_type = typename iterator_traits<iterator_type>::difference_type;
            using pointer = typename iterator_traits<iterator_type>::pointer;
            using reference = typename iterator_traits<iterator_type>::reference;
            using const_point = const pointer;
            using const_reference = const reference;

            Iterator cur_;
        public:
            ReverseListIterator() = default;
            ReverseListIterator(const ReverseListIterator& it): cur_(it.cur_) {}
            explicit ReverseListIterator(const Iterator& it): cur_(it) {}
            ReverseListIterator(ReverseListIterator&& it) noexcept: cur_(std::move(it.cur_)) {}
            explicit ReverseListIterator(Iterator& it): cur_(it) {}
            ~ReverseListIterator() = default;
            ReverseListIterator& operator=(const ReverseListIterator& it) = default;
            ReverseListIterator& operator=(ReverseListIterator&& it) noexcept = default;
            ReverseListIterator& operator=(const Iterator& it) { cur_ = it; return *this;}
            ReverseListIterator& operator=(Iterator&& it) {cur_ = it; return *this; }

            Iterator get() const {return cur_;}

            // *it
            reference operator*() { return *cur_; }
            pointer operator->() { return &operator*(); }

            // ++it
            ReverseListIterator& operator++() { cur_ = operator*().prev();  return *this;}
            // it++
            ReverseListIterator operator++(int) { // it++
                auto tmp = *this;
                ++(*this);
                return tmp;
            }

            ReverseListIterator& operator--() { // --it
                cur_ = operator*().next();
                return *this;
            }
            ReverseListIterator operator--(int) { // it--
                auto tmp = *this;
                --(*this);
                return tmp;
            }

            ReverseListIterator& operator+=(difference_type n) { // it += n
                while (n > 0) {
                    ++(*this);
                    --n;
                }
                return *this;
            }
            ReverseListIterator& operator-=(difference_type n) { // it -= n
                while (n > 0) {
                    --(*this);
                    --n;
                }
                return *this;
            }

            friend ReverseListIterator operator+(const ReverseListIterator& it, difference_type n) { // it + n
                ReverseListIterator tmp = it;
                tmp += n;
                return tmp;
            }
            friend ReverseListIterator operator+(difference_type n, const ReverseListIterator& it) { // n + it
                return it + n;
            }
            ReverseListIterator operator-(difference_type n) { // it - n
                ReverseListIterator tmp = *this;
                tmp -= n;
                return tmp;
            }
            difference_type operator-(const ReverseListIterator& another) { // *this - another
                difference_type n = 0;
                auto tmp = another;
                while (tmp != another and tmp.cur_ != nullptr) {
                    --tmp;
                    ++n;
                }
                if (tmp == *this) return n;
                tmp = *this;
                n = 0;
                while (tmp != another and tmp.cur_ != nullptr) {
                    --tmp;
                    --n;
                }
                return n;
            }

            bool operator==(const ReverseListIterator &another) const {
                return cur_ == another.cur_;
            }
            bool operator!=(const ReverseListIterator &another) const {
                return !(*this == another);
            }

            bool operator<(const ReverseListIterator &another) const {
                auto it = *this;
                while (it != another and it.cur_ != nullptr) {
                    ++it;
                }
                return it == another;
            }
            bool operator>(const ReverseListIterator &another) const {
                return another < *this;
            }
            bool operator<=(const ReverseListIterator &another) const {
                return !(*this > another);
            }
            bool operator>=(const ReverseListIterator &another) const {
                return !(*this < another);
            }
            friend void swap(ReverseListIterator& a, ReverseListIterator& b) noexcept {
                swap(a.cur_, b.cur_);
            }
            void swap(ReverseListIterator& a) noexcept {
                swap(*this, a);
            }
        };

    }

    template <class T>
    class list {
        using node = mzList::ListNode<T>;
        using node_point = node*;
        using nodeAllocator = Allocator<node>;
        using length_type = size_t;
    public:
        using data_type = T;
        using Iterator = mzList::ListIterator<node_point>;
        using ReverseIterator = mzList::ReverseListIterator<node*>;
        using ConstIterator = mzList::ListIterator<const node*>;
        using ConstReverseIterator = mzList::ReverseListIterator<const node*>;

    private:
        Iterator head_{nullptr};
        Iterator tail_{nullptr};
        length_type size_{0};

        list& copy_from(const list& l);
        list& move_from(list&& l) noexcept;

        void allocateAndFill(length_type n, const T& value);
        void allocateAndFill(length_type n, T&& value);
        void allocateAndFill(length_type n);

    public:

        list();
        list(const list& l);  // 拷贝构造函数
        // 移动构造函数
        list(list&& l) noexcept;
        explicit list(length_type n);
        list(length_type n, const T& value);
        list(length_type n, T&& value);
        template<class InputIterator>
        requires(not std::is_integral_v<InputIterator>)
        list(InputIterator first, InputIterator last);
        list(const std::initializer_list<T>& il);
        list(std::initializer_list<T>&& il);
        ~list();  // 析构函数

        // 迭代器相关
        Iterator begin() const {return head_+1;}
        Iterator end() const {return tail_;}
        ConstIterator cbegin() const {return head_+1;}
        ConstIterator cend() const {return tail_;}

        ReverseIterator rbegin() const {return ReverseIterator(tail_-1);}
        ReverseIterator rend() const {return ReverseIterator(head_);}
        ConstReverseIterator crbegin() const {return ConstReverseIterator(tail_-1);}
        ConstReverseIterator crend() const {return ConstReverseIterator(head_);}

        list& operator=(const list& l);
        list& operator=(list&& l) noexcept;
        list& operator=(const std::initializer_list<T>& il);
        list& operator=(std::initializer_list<T>&& il);

        bool operator==(const list &) const;
        bool operator!=(const list &another) const {return !(*this == another);}

        // 获取元素，效率很慢的
        T& operator[](length_type index);  // 保证复杂度在 O(n/2) 完成

        // 插入与删除元素
        void push_back(const T& value);
        void push_back(T&& value);
        void push_front(const T& value);
        void push_front(T&& value);
        bool pop_back();
        bool pop_front();

        // insert插入元素
        template<class InputIterator> void insert(InputIterator it, const T& value);
        template<class InputIterator> void insert(InputIterator it, T&& value);
        template<class InputIterator> void insert(InputIterator it, length_type n, const T& value);
        template<class InputIterator> void insert(InputIterator it, length_type n, T&& value);
        template<class InputIterator, class InputIterator2>
        requires(not std::is_integral_v<InputIterator>)
        void insert(InputIterator it, InputIterator2 first, InputIterator2 last);

        // 删除元素
        template<class InputIterator> void erase(InputIterator it);
        template<class InputIterator> void erase(InputIterator first, InputIterator last);

        // 获取大小
        [[nodiscard]] length_type size() const {return size_;}
        [[nodiscard]] bool empty() const {return size_ == 0;}

        template<class InputIterator, class InputIterator2>
        list& merge(InputIterator it, InputIterator2 first, InputIterator2 last, length_type n);

        // swap
        friend void swap(list& l1, list& l2) noexcept {
            tinyWheels::swap(l1.head_, l2.head_);
            tinyWheels::swap(l1.tail_, l2.tail_);
            tinyWheels::swap(l1.size_, l2.size_);
        }

        void swap(list& l) noexcept {
            swap(*this, l);
        }

        friend std::ostream& operator<<(std::ostream& os, const list& l) {
            if constexpr (is_ostream_writable_v<T>) {
                for (auto it = l.begin(); it != l.end(); ++it) {
                    if (it != l.begin()) os << ", ";
                    os << (*it).data();
                }
                os << " (" << l.size_ << ")";
            }else {
                os << "list<" << typeid(T).name() << ">" << "size: " << l.size_;
            }
            return os;
        }
    };
}

#endif //LIST_DEF_H
