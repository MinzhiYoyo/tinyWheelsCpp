//
// Created by 24983 on 25-2-23.
//

#ifndef MYSTRINGS_H
#define MYSTRINGS_H

#include "allocator.h"

namespace tinyWheels{
    class string {
    private: // 定义类型
        using size_type = size_t;
        using char_type = char;
        using char_point = char_type*;
        using c_string = const char_type*;
        using const_char_point = const char_type*;
        using charAllocator = Allocator<char_type>;
    public:
        using Iterator = char_point;
        using ConsIterator = const_char_point;
        using ReverseIterator = std::reverse_iterator<Iterator>;
        using ConsReverseIterator = std::reverse_iterator<ConsIterator>;
    private: // 定义变量
        char_point data_{nullptr};
        size_type size_{0};   // 包括了'\0'
        size_type capacity_{0};

    private: // 定义函数
        void copy_from(const string& str);
        void move_from(string&& str) noexcept;
        static void move_forward(Iterator first, const Iterator last, Iterator dst);  // 向前移动
        static void move_back(const ReverseIterator &first, const ReverseIterator& last, const ReverseIterator& dst); // 向后移动
    public:

        [[nodiscard]] size_type size() const {return size_ - 1 < 0 ? 0 : size_ - 1;}
        [[nodiscard]] size_type length() const {return size();}
        [[nodiscard]] Iterator begin() const {return data_;}
        [[nodiscard]] Iterator end() const {return data_ + size();}
        [[nodiscard]] ConsIterator cbegin() const {return data_;};
        [[nodiscard]] ConsIterator cend() const {return data_ + size();}
        [[nodiscard]] ReverseIterator rbegin() const {return ReverseIterator(end());}
        [[nodiscard]] ReverseIterator rend() const {return ReverseIterator(begin());}
        [[nodiscard]] ConsReverseIterator crbegin() const {return ConsReverseIterator(cend());}
        [[nodiscard]] ConsReverseIterator crend() const {return ConsReverseIterator(cbegin());}


        ~string();
        string() = default;
        string(const string&);
        string(string&&) noexcept;
        string(char_type);
        string(c_string);
        string(size_type, char_type);
        string(size_type, c_string);

        [[nodiscard]] c_string c_str() const {return data_;}
        [[nodiscard]] char_type operator[](const size_type n) const {
            if (n >= size_) {
                throw tinyWheels::exception("超出范围, index: %lu, size: %lu", n, size_);
            }
            return data_[n];
        }

        void insert(Iterator it, const string& s);
        void insert(Iterator it, char_type);
        void insert(Iterator it, c_string);

        string& operator=(const string&);
        string& operator=(string&&) noexcept ;
        string& operator=(char_type);
        string& operator=(c_string);

        string& operator+=(const string&);
        string& operator+=(char_type);
        string& operator+=(c_string);
        // template<class Number>
        // string& operator+=(Number);


        friend string operator+(const string&, const string&);
        friend string operator+(const string&, string&&);
        friend string operator+(string&&, const string&);
        friend string operator+(string&&, string&&) noexcept;

        friend string operator+(const string&, char_type);
        friend string operator+(char_type, const string&);
        friend string operator+(string&&, char_type) noexcept;
        friend string operator+(char_type, string&&) noexcept;

        friend string operator+(const string&, c_string);
        friend string operator+(c_string, const string&);
        friend string operator+(string&&, c_string) noexcept;
        friend string operator+(c_string, string&&) noexcept;

        friend bool operator==(const string &, const string &);
        friend bool operator==(const string &, c_string);
        friend bool operator==(c_string, const string &);
        friend bool operator==(const string &, char_type);
        friend bool operator==(char_type, const string&);

        friend bool operator!=(const string &, const string &);
        friend bool operator!=(const string &, c_string);
        friend bool operator!=(c_string, const string &);
        friend bool operator!=(const string &, char_type);
        friend bool operator!=(char_type, const string&);

        friend bool operator>(const string &, const string &);
        friend bool operator>(const string &, c_string);
        friend bool operator>(c_string, const string &);

        friend bool operator>=(const string &, const string &);
        friend bool operator>=(const string &, c_string);
        friend bool operator>=(c_string, const string &);

        friend bool operator<(const string &, const string &);
        friend bool operator<(const string &, c_string);
        friend bool operator<(c_string, const string &);

        friend bool operator<=(const string &, const string &);
        friend bool operator<=(const string &, c_string);
        friend bool operator<=(c_string, const string &);

        // template<class Number>
        // friend string& operator+(const string&, Number);
        // template<class Number>
        // friend string& operator+(string&&, Number) noexcept;
        void reserve(size_type);
    };
}

#endif //MYSTRINGS_H
