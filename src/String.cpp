#include "algorithm.h"
#include <cstring>
#include <reverse_iterator.h>

#include "mystring.h"

namespace tinyWheels{
    void string::copy_from(const string &str) {
        const auto s = str.size();    // 字符串大小
        const auto c = s + 1;  // 最低容量
        if (capacity_ < c) {
            auto [ptr, cap] = charAllocator::allocate(c);
            data_ = ptr;
            capacity_ = cap;
        }
        char_point ps = str.data_, pd = data_;
        for (; *ps != '\0'; ++ps, ++pd) {
            *pd = *ps;
        }
        *pd = '\0';
        size_ = s + 1;
    }

    void string::move_from(string &&str) noexcept {
        data_ = str.data_;
        size_ = str.size_;
        capacity_ = str.capacity_;
        str.data_ = nullptr;
        str.size_ = 0;
        str.capacity_ = 0;
    }

    void string::move_forward(const Iterator first, const Iterator last, const Iterator dst) {
        for (auto it1 = first, it2 = dst; it1 != last; ++it1, ++it2) {
            *it2 = *it1;
        }
    }

    void string::move_back(const ReverseIterator &first, const ReverseIterator& last, const ReverseIterator& dst) {
        for (auto it1 = first, it2 = dst; it1 != last; ++it1, ++it2) {
            *it2 = *it1;
        }
    }

    string::~string() {
        if (data_) {
            charAllocator::deallocate(data_, capacity_);
            size_ = 0;
            capacity_ = 0;
        }
    }

    string::string(const string &str) {
        copy_from(str);
    }

    string::string(string &&str) noexcept {
        move_from(std::forward<string>(str));
    }

    string::string(const char_type ch) {
        reserve(1);  // 初始化容量为1
        data_[0] = ch;
        data_[1] = '\0';
    }

    string::string(c_string c_str_point) {
        auto len = strlen(c_str_point);
        reserve(len + 1);
        for (size_type i = 0; i < len; ++i) {
            data_[i] = c_str_point[i];
        }
    }

    string::string(size_type st, char_type ch) {
        reserve(st + 1);
        fill(data_, data_ + st, ch);
        data_[st] = '\0';
    }

    string::string(size_type st, c_string c_str_point) {
        auto len = strlen(c_str_point);
        reserve(st * len + 1);
        for (size_type i = 0; i < st; ++i) {
            for (size_type j = 0; j < len; ++j) {
                data_[i * len + j] = c_str_point[j];
            }
        }
        data_[st * len] = '\0';
    }

    void string::insert(Iterator it, const string &s) {
        const auto len = s.size();
        const auto max_len = len + size() + 1;
        if (capacity_ < max_len) {
            reserve(max_len);
        }
        const auto first = rbegin();
        const auto last = static_cast<ReverseIterator>(it)+1;
        // 把对应位置向后移动
        const auto dst = rbegin() - static_cast<tinyWheels::ReverseIterator<char_type>::difference_type>(len);
        move_back(first, last, dst);
        for (size_type i = 0; i < len; ++i) {
            *it++ = s.data_[i];
        }
        size_ = max_len;
        data_[size_-1] = '\0';
    }

    void string::insert(Iterator it, char_type ch) {
        const auto max_len = size() + 2;
        if (capacity_ < max_len) {
            reserve(max_len);
        }
        const auto first = rbegin();
        const auto last = static_cast<ReverseIterator>(it)+1;
        const auto dst = rbegin() - 1;
        move_back(first, last, dst);
        *it = ch;
        size_ = max_len;
        data_[size_-1] = '\0';
    }
    void string::insert(Iterator it, c_string cs) {
        const auto len = strlen(cs);
        const auto max_len = len + size() + 1;
        if (capacity_ < max_len) {
            reserve(max_len);
        }
        const auto first = rbegin();
        const auto last = static_cast<ReverseIterator>(it)+1;
        const auto dst = rbegin() - static_cast<tinyWheels::ReverseIterator<char_type>::difference_type>(len);
        move_back(first, last, dst);
        for (size_type i = 0; i < len; ++i) {
            *it++ = cs[i];
        }
        size_ = max_len;
        data_[size_-1] = '\0';
    }



    string &string::operator=(const string &str) {
        if (this != &str) {
            copy_from(str);
        }
        return *this;
    }

    string &string::operator=(string &&str) noexcept {
        if (this != &str) {
            move_from(std::forward<string>(str));
        }
        return *this;
    }

    string &string::operator=(char_type ch) {
        if (capacity_ < 2) {
            reserve(2);
        }
        data_[0] = ch;
        data_[1] = '\0';
        size_ = 2;
        return *this;
    }

    string &string::operator=(c_string c_str_point) {
        auto len = strlen(c_str_point);
        if (capacity_ < len + 1) {
            reserve(len + 1);
        }
        for (size_type i = 0; i < len; ++i) {
            data_[i] = c_str_point[i];
        }
        data_[len] = '\0';
        size_ = len + 1;
        return *this;
    }

    string &string::operator+=(const string &str) {
        const auto size_me = size();
        const auto size_str = str.size();
        if (capacity_ < size_me + size_str + 1) {
            reserve(size_me + size_str + 1);
        }
        char_point ps = str.data_, pm = data_ + size_me;
        for (; *ps != '\0'; ++ps, ++pm) {
            *pm = *ps;
        }
        *pm = '\0';
        size_ = size_me + size_str + 1;
        return *this;
    }

    string &string::operator+=(char_type ch) {
        const auto size_me = size();
        if (capacity_ < size_me + 2) {
            reserve(size_me + 2);
        }
        data_[size_me] = ch;
        data_[size_me + 1] = '\0';
        size_ = size_me + 2;
        return *this;
    }

    string &string::operator+=(c_string c_str_point) {
        const auto size_me = size();
        const auto size_str = strlen(c_str_point);
        if (capacity_ < size_me + size_str + 1) {
            reserve(size_me + size_str + 1);
        }
        char_point ps = const_cast<char_point>(c_str_point), pm = data_ + size_me;
        for (; *ps != '\0'; ++ps, ++pm) {
            *pm = *ps;
        }
        *pm = '\0';
        size_ = size_me + size_str + 1;
        return *this;
    }

    string operator+(const string& s1, const string& s2) {
        string s(s1);
        s += s2;
        return s;
    }
    string operator+(const string& s1, string&& s2) {
        s2.insert(s2.begin(), s1);
        return s2;
    }
    string operator+(string&& s1, const string& s2) {
        s1 += s2;
        return s1;
    }
    string operator+(string&& s1, string&& s2) noexcept {
        s1 += s2;
        return s1;
    }

    string operator+(const string& s, string::char_type ch) {
        string s1(s);
        s1 += ch;
        return s1;
    }
    string operator+(string::char_type ch, const string& s) {
        string s1(s);
        s1.insert(s1.begin(), ch);
        return s1;
    }
    string operator+(string&& s, string::char_type ch) {
        s += ch;
        return s;
    }
    string operator+(string::char_type ch, string&& s) {
        s.insert(s.begin(), ch);
        return s;
    }

    string operator+(const string& s1, string::c_string s2) {
        string s(s1);
        s += s2;
        return s;
    }
    string operator+(string::c_string s1, const string& s2) {
        string s(s1);
        s += s2;
        return s;
    }
    string operator+(string&& s1, string::c_string s2) {
        s1 += s2;
        return s1;
    }
    string operator+(string::c_string s1, string&& s2) {
        s2.insert(s2.begin(), s1);
        return s2;
    }

    bool operator==(const string & s1, const string & s2) {
        if (s1.size() != s2.size()) return false;
        for (int i = 0; i < s1.size(); ++i) {
            if (s1.data_[i] != s2.data_[i]) return false;
        }
        return true;
    }
    bool operator==(const string & s1, string::c_string s2) {
        int i = 0;
        for (; s2[i] != '\0'; ++i) {
            if (s1.data_[i] != s2[i]) return false;
        }
        return s1.data_[i] == '\0';
    }
    bool operator==(string::c_string s1, const string& s2) {
        return s2 == s1;
    }
    bool operator==(const string & s1, string::char_type ch) {
        return s1.size() == 1 and s1.data_[0] == ch;
    }
    bool operator==(string::char_type ch, const string& s2) {
        return s2 == ch;
    }

    bool operator!=(const string & s1, const string & s2) {
        return not(s1 == s2);
    }
    bool operator!=(const string & s1, string::c_string s2) {
        return not(s1 == s2);
    }
    bool operator!=(string::c_string s1, const string& s2) {
        return not(s1 == s2);
    }
    bool operator!=(const string & s1, string::char_type ch) {
        return not(s1 == ch);
    }
    bool operator!=(string::char_type ch, const string& s2) {
        return not(ch == s2);
    }

    bool operator>(const string & s1, const string & s2) {
        for (int i = 0; i < s1.size(); ++i) {
            if (s1.data_[i] > s2.data_[i]) return true;
            if (s1.data_[i] < s2.data_[i]) return false;
        }
        return false;
    }
    bool operator>(const string & s1, string::c_string s2) {
        int i = 0;
        for (; s2[i] != '\0'; ++i) {
            if (s1.data_[i] > s2[i]) return true;
            if (s1.data_[i] < s2[i]) return false;
        }
        return s1.data_[i] > '\0';
    }
    bool operator>(string::c_string s1, const string& s2) {
        int i = 0;
        for (; s1[i] != '\0'; ++i) {
            if (s1[i] > s2.data_[i]) return true;
            if (s1[i] < s2.data_[i]) return false;
        }
        return false;
    }


    bool operator>=(const string & s1, const string & s2) {
        int i = 0;
        for (; s2[i] != '\0'; ++i) {
            if (s1.data_[i] > s2[i]) return true;
            if (s1.data_[i] < s2[i]) return false;
        }
        return s1.data_[i] >= '\0';
    }
    bool operator>=(const string & s1, string::c_string s2) {
        int i = 0;
        for (; s2[i] != '\0'; ++i) {
            if (s1.data_[i] > s2[i]) return true;
            if (s1.data_[i] < s2[i]) return false;
        }
        return s1.data_[i] >= '\0';
    }
    bool operator>=(string::c_string s1, const string& s2) {
        int i = 0;
        for (; s1[i] != '\0'; ++i) {
            if (s1[i] > s2.data_[i]) return true;
            if (s1[i] < s2.data_[i]) return false;
        }
        return s2.data_[i] == '\0';
    }


    bool operator<(const string & s1, const string & s2) {
        return s2 > s1;
    }
    bool operator<(const string & s1, string::c_string s2) {
        return s2 > s1;
    }
    bool operator<(string::c_string s1, const string& s2) {
        return s2 > s1;
    }

    bool operator<=(const string & s1, const string & s2) {
        return s2 >= s1;
    }
    bool operator<=(const string & s1, string::c_string s2) {
        return s2 >= s1;
    }
    bool operator<=(string::c_string s1, const string& s2) {
        return s2 >= s1;
    }


    void string::reserve(const size_type c) {
        if (capacity_ < c) {
            auto [ptr, cap] = charAllocator::allocate(c);
            char_point ps = data_, pm = ptr;
            for (; *ps != '\0'; ++ps, ++pm) {
                *pm = *ps;
            }
            *pm = '\0';
            charAllocator::deallocate(data_, capacity_);
            data_ = ptr;
            capacity_ = cap;
        }
    }





}
