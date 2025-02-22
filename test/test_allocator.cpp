#include <iostream>
#include "allocator.h"

class TestAllocator {
    int id = 0;
    const char *name = "hello world";
public:
    TestAllocator() = default;
    TestAllocator(const int id, const char *name) : id(id), name(name) {}
    ~TestAllocator() = default;
    void say() const {
        std::cout << "id = " << id << ", name = " << name << std::endl;
    }
    friend std::ostream& operator<<(std::ostream& os, const TestAllocator& obj) {
        os << "(" << obj.id << ", \"" << obj.name << "\")";
        return os;
    }
};

template <class T>
void printT(const T* ptr, size_t size);

template <class T, class... Args>
void TestT(const std::initializer_list<int>& list, Args... args) {
    // 通过不断生成对象来测试内存分配器，并查看分配器的一些地址情况
    // list存储着每次分配的对象的地址
    for (auto number : list) {
        auto [ptr, cap] = tinyWheels::Allocator<T>::allocate(number);
        tinyWheels::Allocator<T>::construct(ptr, number, args...);
        printT(ptr, number);
        tinyWheels::Allocator<T>::Destruct(ptr, number);
        tinyWheels::Allocator<T>::deallocate(ptr, number);
    }
}

template <class T>
void printT(const T* ptr, const size_t size) {
    for (size_t i = 0; i < size; ++i) {
        std::cout << ptr[i] << " ";
    }
    std::cout << std::endl;
}

int main() {
    // 通过测试 int float 自定义类型的内存分配来测试内存分配器
    TestT<int>({1, 2, 3, 4, 5, 10, 50}, 114514);
    TestT<int>({10, 12, 14, 16, 18, 20, 22}, 1514);

    TestT<int>({1, 2, 3, 4, 5, 10, 50, 100}, 1919810);
    TestT<float>({1, 2, 3, 4, 5, 10, 50, 100}, 3.1415926);
    TestT<TestAllocator>({1, 2, 3, 4, 5, 10, 50, 100}, 10, "测试");
    return 0;
}
