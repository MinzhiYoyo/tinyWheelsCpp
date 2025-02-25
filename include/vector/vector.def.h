//
// Created by 24983 on 25-2-22.
//

#ifndef VECTOR_DEFINE_H
#define VECTOR_DEFINE_H
// #include <initializer_list>
#include <iosfwd>
#include "allocator.h"
#include "traits.h"
#include "iterator.h"
#include "reverse_iterator.h"
/**
 * vector需要实现的功能：
 *
 */
namespace tinyWheels{
    template<class T, class Alloc = Allocator<T>>
    // class vector : public base_container<T> {
    class vector{
    public:
        using dataAllocator = Alloc;
        using length_type = size_t;
    private:
        T* data_{nullptr};
        length_type size_{0};
        length_type capacity_{0};

        template<class InputIterator>
        InputIterator recapacity(length_type new_size, InputIterator it); // 数据迁移
        void allocateAndFill(length_type n, const T& value);
        void allocateAndFill(length_type n, T&& value);
        void allocateAndFill(length_type n);

        // 移动数据，如果是使用正向迭代器，那么就是将数据向前移动，如果使用反向迭代器，那么就是将数据向后移动
        template<class InputIterator>
        void move_data(InputIterator first, InputIterator last, InputIterator dst); // 移动数据
    public:
        using Iterator = T*;
        using ConstIterator = const T*;
        using reverseIterator = ReverseIterator<T*>;
        using constReverseIterator = ReverseIterator<const T*>;


        void resize(length_type s);

        // 构造函数与析构函数
        vector();
        vector(const vector&);
        vector(vector&&) noexcept ;
        explicit vector(length_type n);
        vector(length_type n, const T& value);
        vector(length_type n, T&& value);
        vector(const std::initializer_list<T>& il);  // 初始化列表构造实现
        vector(std::initializer_list<T>&& il);       // 初始化列表构造实现
        template<class InputIterator>
        vector(InputIterator first, InputIterator last);  // 迭代器构造实现
        ~vector() ;

        vector& operator=(const vector&);
        vector& operator=(vector&&) noexcept ;
        vector& operator=(const std::initializer_list<T>& il);
        vector& operator=(std::initializer_list<T>&& il);

        [[nodiscard]] length_type size() const  {return size_;}  // 返回容器中元素的个数
        [[nodiscard]] length_type capacity() const {return capacity_;}  // 返回容器的容量
        [[nodiscard]] bool empty() const {return size_ == 0;}  // 判断容器是否为空

        vector& move_from(vector&&) noexcept ;
        vector& copy_from(const vector&);

        bool operator==(const vector &) const;
        bool operator!=(const vector &) const;

        void push_back(const T& value);
        void push_back(T&& value);
        template<class... Args>
        void emplace_back(Args&&... args);
        bool pop_back();

        // 插入元素
        template<class InputIterator>
        void insert(InputIterator it, const T& value);
        template<class InputIterator>
        void insert(InputIterator it, T&& value);
        template<class InputIterator>
        void insert(InputIterator it, length_type n, const T& value);
        template<class InputIterator>
        void insert(InputIterator it, length_type n, T&& value);
        template<class InputIterator, class InputIterator2>
        requires(not std::is_integral_v<InputIterator2>)
        void insert(InputIterator it, InputIterator2 first, InputIterator2 last);

        // 删除元素
        template<class InputIterator>
        void erase(InputIterator it);
        template<class InputIterator>
        void erase(InputIterator first, InputIterator last);

        // 获取元素
        T& operator[](length_type index) {
            if (index >= capacity_) {
                throw exception("Out of range, index: %lu, size: %lu, capacity: %lu", index, size_, capacity_);
            }
            return data_[index];
        }
        T& back() {
            return data_[size_ - 1];
        }

        Iterator begin() const {return data_;}  // 返回指向容器中第一个元素的迭代器
        ConstIterator cbegin() const {return data_;}  // 返回指向容器中第一个元素的迭代器
        Iterator end() const {return data_ + size_;}  // 返回指向容器中最后一个元素的迭代器
        ConstIterator cend() const {return data_ + size_;}  // 返回指向容器中最后一个元素的迭代器
        reverseIterator rbegin() const {return reverseIterator(end());}
        constReverseIterator crbegin() const {return constReverseIterator(end());}
        reverseIterator rend() const {return reverseIterator(begin());}
        constReverseIterator crend() const {return constReverseIterator(begin());}

        friend std::ostream& operator<<(std::ostream& os, const vector<T>& vec) {
            if constexpr (is_ostream_writable_v<T>) {
                for (auto it = vec.begin(); it != vec.end(); ++it) {
                    if (it != vec.begin()) os << ", ";
                    os << *it;
                }
                os << " (" << vec.size_ << ")";
            }else {
                os << "vector<`" << typeid(T).name() << "`>" << " size: " << vec.size() << " capacity: " << vec.capacity_ << " data: " << vec.data_;
            }
            return os;
        }
        friend void swap(vector&v1, vector&v2) noexcept {
            tinyWheels::swap(v1.data_, v2.data_);
            tinyWheels::swap(v1.size_, v2.size_);
            tinyWheels::swap(v1.capacity_, v2.capacity_);
        }
        void swap(vector& v) noexcept {
            tinyWheels::swap(*this, v);
        }
    };
}
#endif //VECTOR_DEFINE_H
