#include <string>
#include <iostream>
using namespace std;

class A {
    int a_{0};
public:
    A() {
        cout << "默认构造函数" << endl;
    }
    explicit A(const int a): a_(a) {
        cout << "带参构造: " << a << endl;
    }
    A(const A& a) {
        a_ = a.a_;
        cout << "拷贝构造：" << a_ << endl;
    }
    A(A&& a) noexcept {
        a_ = a.a_;
        cout << "移动构造：" << a_ << endl;
    }
    ~A() {
        cout << "析构函数: " << a_ << endl;
    }
    A& operator=(const A& a) {
        a_ = a.a_;
        cout << "拷贝赋值：" << a_ << endl;
        return *this;
    }
    A& operator=(A&& a) noexcept {
        cout << "移动赋值";
        if (this != &a) {
            a_ = a.a_;
            a.a_ = -100;
            cout << " 移动了" ;
        }
        cout << " " << a_ << endl;
        return *this;
    }
};

A fun_value(A&& a) {

    // return std::move(a);
    return a;
    // return std::forward<A>(a);
}


int main() {
    A a = fun_value(A(100));
    return 0;
}
