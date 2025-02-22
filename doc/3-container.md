# 第三章-容器
迭代器和容器是STL的基础东西，任何装数据的都叫做容器，需要实现：vector, list, queue, stack, deque, set, map, unordered_set, unordered_map等等。

在本节中，讲讲容器最基本的功能实现。

# vector

vector是一个动态且支持随机访问的容器，底层是一个数组，可以通过下标访问元素，也可以通过迭代器访问元素。

其需要实现以下功能：

1. 构造函数与赋值运算符

```cpp
vector();  // 默认的构造函数
vector(length); // 构造一个长度为length，且元素默认初始化的构造函数
vector(length, value); // 构造一个长度为length，元素均为value的构造函数
vector(vec); // 用另一个vector来进行构造
vector(initializer_list); // 使用 std::initializer_list 来构造
vector(first, last); // 使用迭代器来构造

vector& operator=(const vector&);
vector& operator=(vector&&) noexcept ;
vector& operator=(const std::initializer_list<T>& il);
vector& operator=(std::initializer_list<T>&& il);
```

2.   析构函数

```cpp
~vector();
```

3.   成员变量

```cpp
T* data_;  // 保存数据的数组
size_t size_; // 数据大小
size_t capacity_; // 实际容量
```

在申请内存的时候，默认会多申请部分容量，这是由于数组有可能是动态了，而且数组释放之后，内存会被释放的，因此不用担心

4.   插入与删除

```cpp
// 插入元素
void insert(InputIterator it, const T& value);
void insert(InputIterator it, T&& value);
void insert(InputIterator it, size_t n, const T& value);
void insert(InputIterator it, size_t n, T&& value);
void insert(InputIterator it, InputIterator2 first, InputIterator2 last);

// 删除元素
void erase(InputIterator it);
void erase(InputIterator first, InputIterator last);
```

5.   获取元素

```cpp
T& operator[](length_type index)
```

6.   迭代器相关

```cpp
using Iterator = T*;
using ConstIterator = const T*;
using reverseIterator = ReverseIterator<T*>;
using constReverseIterator = ReverseIterator<const T*>;
Iterator begin() {return data_;}  // 返回指向容器中第一个元素的迭代器
ConstIterator cbegin() const {return data_;}  // 返回指向容器中第一个元素的迭代器
Iterator end() {return data_ + size_;}  // 返回指向容器中最后一个元素的迭代器
ConstIterator cend() const {return data_ + size_;}  // 返回指向容器中最后一个元素的迭代器
reverseIterator rbegin() {return reverseIterator(end());}
constReverseIterator rcbegin() const {return constReverseIterator(end());}
reverseIterator rend() {return reverseIterator(begin());}
constReverseIterator rcend() const {return constReverseIterator(begin());}
```

# list
list是一个双向链表，支持双向迭代，但不支持随机访问（可以使用$O(n)$的复杂度实现一个）。
要点在于，先定义好节点类，再定义迭代器类，然后再来定义list类，值得注意的是，增加两个头尾节点可以使得list书写起来更加简洁高效。

1.   节点类

```cpp
template <class T>
class ListNode {
    T data_;
    ListNode*prev_ = nullptr;
    ListNode*next_ = nullptr;
public:
    // 增加构造函数以及比较重要的是，增加set函数，由于我们定义的 data_,prev_和next_是私有变量，所以需要增加这个
};
```

2.   正向迭代器与反向迭代器

```cpp
template <class Iterator>
class ListIterator {
    Iterator cur_;  // list的迭代器我采用的是 ListNode *
public:
    // 构造函数与赋值函数就不做过多强调
    // 解引用
    reference operator*() { return *cur_; }
    pointer operator->() {  return &operator*();}
	
    // 自增自减运算符需要结合 ListNode 来实现
    ListIterator& operator++() {  // ++it
        cur_ = operator*().next();
        return *this;
    }
    // 略
	
    // 需要循环实现
    ListIterator& operator+=(difference_type n) { // it += n
        while (n > 0) {
            ++(*this);
            --n;
        }
        return *this;
    }
	
    // 计算距离，一般来说list是不需要计算距离的，但是其实会用到这个，所以也写上去了
    // 思路是，从 another 往前面遍历
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
        return n;  // 这里是负
    }
};

template <class Iterator>
class ReverseListIterator;// 基本上同理实现
```

3.   list类

```cpp
  template <class T>
    class list {
        using node = mzList::ListNode<T>;
        using node_point = node*;
        using nodeAllocator = Allocator<node>;  // 内存分配
    public:
        using data_type = T;
        using Iterator = mzList::ListIterator<node_point>;   // 定义了迭代器
		// 其他省略

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
        template<class InputIterator> list(InputIterator first, InputIterator last);
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
        template<class InputIterator, class InputIterator2> void insert(InputIterator it, InputIterator2 first, InputIterator2 last);
        // 删除元素
        template<class InputIterator> void erase(InputIterator it);
        template<class InputIterator> void erase(InputIterator first, InputIterator last);

        // 获取大小
        [[nodiscard]] length_type size() const {return size_;}
        [[nodiscard]] bool empty() const {return size_ == 0;}

        template<class InputIterator, class InputIterator2>
        list& merge(InputIterator it, InputIterator2 first, InputIterator2 last, length_type n);
    };
```

这里没有参考其他东西，所以list我采取与vector相同的接口，vector能用的，list也基本能用，但是底层实现不一样，效率也不一样。
