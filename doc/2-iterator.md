# 第2课-迭代器
# 介绍
STL迭代器是STL的重要组成部分，它是STL的核心之一。迭代器是一种对象，它允许我们遍历容器中的元素。迭代器是一种智能指针，它可以指向容器中的元素，我们可以通过迭代器来访问容器中的元素。迭代器是STL的基础，它是STL的核心之一。

# 迭代器的种类
## 五种迭代器
STL中有五种迭代器，分别是：
1. InputIterator：输入迭代器，对容器只读，只能通过++向前移动，不能向后移动，只能使用一次。
2. OutputIterator：输出迭代器，对容器只写，只能通过++向前移动，不能向后移动，只能使用一次。
3. ForwardIterator：前向迭代器，对容器可读可写，可以通过++向前移动，不能向后移动，可以多次使用。
4. BidirectionalIterator：双向迭代器，可以通过++向前移动，可以通过--向后移动，可以多次使用。
5. RandomAccessIterator：随机访问迭代器，支持随机访问，可以通过+、-、+=、-=、[]、<、>、<=、>=等操作。

下面展示了迭代器的继承关系：
```text
InputIterator -> ForwardIterator -> BidirectionalIterator -> RandomAccessIterator
OutputIterator
```

# 迭代器模板
## 迭代器标签与模板

迭代器标签也就是迭代器的种类，用于区分迭代器种类的，STL中定义了如下迭代器标签：
```cpp
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
```

基于上述五种标签，STL定义了五种迭代器类模板，分别是：`input_iterator`、`output_iterator`、`forward_iterator`、`bidirectional_iterator`、`random_access_iterator`。

## 迭代器类型萃取(traits)

迭代器类型萃取是STL中的一个重要概念，它是通过模板特化来实现的，通过迭代器类型萃取，我们可以获取迭代器的种类，从而实现不同种类迭代器的操作。比如 vector 和 list 的迭代器种类是不一样的，list不支持随机访问，而 vector 支持随机访问。

而指针也是一种随机访问迭代器，并且天然支持++，--等操作，甚至都不需要重载，因此对指针迭代器进行实例化，会简单很多。

```cpp
// 迭代器类型萃取
template<class Iterator>
struct iterator_traits
{
    using iterator_category = typename Iterator::iterator_category;
    using value_type = typename Iterator::value_type;
    using difference_type = typename Iterator::difference_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;
};
// 对指针类型的特化，由于指针天生就是随机访问迭代器，甚至都不需要定义++ --等操作，所以直接特化，加速访问
template<class T>
struct iterator_traits<T*>
{
    using iterator_category = random_access_iterator_tag;  // 迭代器类型
    using value_type = T;                                  // 迭代器所指对象的类型
    using difference_type = ptrdiff_t;                     // 两个迭代器之间的距离
    using pointer = T*;                                    // 迭代器所指对象的指针
    using reference = T&;                                  // 迭代器所指对象的引用
};
template<class T>
struct iterator_traits<const T*>
{
    using iterator_category = random_access_iterator_tag;  // 迭代器类型
    using value_type = T;                                  // 迭代器所指对象的类型
    using difference_type = ptrdiff_t;                     // 两个迭代器之间的距离
    using pointer = const T*;                              // 迭代器所指对象的指针
    using reference = const T&;                            // 迭代器所指对象的引用
};
```

# 正向迭代器和反向迭代器
正向迭代器需要实现的功能是，从首元素出发，一直遍历到末元素的后面位置。那么反向迭代器的功能需要实现，从末元素出发，一直遍历到首元素的前一个位置。

# 迭代器需要实现的功能
```cpp
template<class T>
class Iterator{
private:
    T cur_;
public:
    base_iterator() = default;
    ~base_iterator() = default;
    
    // 重载++--运算符
    Iterator& operator++();
    Iterator operator++(int);
    Iterator& operator--();
    Iterator operator--(int);
    
    // 重载 += -= 运算符
    Iterator& operator+=(int n);
    Iterator& operator-=(int n);
    
    // 重载+ - 运算符，需要实现 int + Iterator 和 Iterator + int，所以用友元方式重载
    friend Iterator operator+(const Iterator& it, int n);
    friend Iterator operator+(int n, const Iterator& it);
    Iterator operator-(int n); // 只需要实现 Iterator - int 即可
    int operator-(const Iterator& it); // 实现两个指针之间的距离
    
    // 重载 == != 运算符
    bool operator==(const Iterator& it);
    bool operator!=(const Iterator& it);
    // 重载 < > <= >= 
    bool operator<(const Iterator& it);
    bool operator>(const Iterator& it);
    bool operator<=(const Iterator& it);
    bool operator>=(const Iterator& it);
    
    // 重载*运算符
    T& operator*();
    // 重载->运算符
    T* operator->();
    
    // 获取地址
    T get() const {return cur_;}
};
```

# 迭代器的容器应用
把容器分成两种，一种是连续存储的容器，一种是非连续存储的容器。迭代器根据方向又分为两种，前向迭代器和反向迭代器。

## 连续容器

```cpp
// 假设已经定义了迭代器类，Iterator<>与ReverseIterator<>
template <class T>
class ContinuousContainer{
public:
    using Iterator = Iterator<T*>;  // 直接使用元素的指针T*作为前向迭代器即可
    using ReverseIterator = ReverseIterator<T*>;  // 反向迭代器也是同理的
    // 基于上述两个定义，我们就可以使用 ContinuousContainer::Iterator 类型表示 ContinuousContainer 的前向迭代器
}
```

## 非连续容器

```cpp
template <class T>
class NonContinuousContainer{
    using Iterator = ListIterator<T*>;
    using ReverseIterator = ReverseListIterator<T*>;
};
```

