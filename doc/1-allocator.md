# 内存分配器

# 介绍

分配器(allocator)是c++标准库的一个组件，主要是用来标准库的容器内存的分配与释放的，对于小内存的申请，如果频繁调用`new`或`malloc`可能会造成内存碎片，导致内存的浪费以及时间的浪费。



# 基本接口

需要实现核心功能如下：

-   内存分配：分配大小为 memory_bytes 的内存块，通过计算 number * sizeof(T) 来获取内存块大小
-   内存回收：释放内存块，释放大小为 number 的内存块，起始地址为 start_memory
-   对象构造：在起始地址为 start_memory 的内存块上构造 number 个对象，args为构造对象的参数
-   对象析构：在起始地址为 start_memory 的内存块上析构 number 个对象

# 整体实现

## 内存分配

简单思路就是，申请一大段内存资源，然后使用伙伴算法来分配小内存块，大内存块直接向操作系统申请

1.   计算对齐内存，这表示需要申请的内存块，这个函数将会申请这一块内存
2.   判断未对齐内存是否超过阈值，超过了直接向操作系统申请，否则使用伙伴算法
3.   伙伴算法：通过使用自由链表的方式来实现
     1.   判断自由链表是否还有剩余的对应内存块，有的话，申请一块返回即可
     2.   没有的话，判断段内存资源是否还足够申请 BLOCK_NUMBER 个内存块，够的话把这部分内存块加入到自由链表中
     3.   不够的话，判断内存资源是否还足够申请 1 个内存块，够的话把这部分内存加入到自由链表中
     4.   还不够的话，调用`chunk_memory()`再次申请一大段内存资源

```cpp
static T *allocate(variable_count  number = 1);
// 内存分配
template <class T>
T *Allocator<T>::allocate(const variable_count number) {
    ...
        
    // 计算需要的内存
    const memory_size_type memory_bytes = number * sizeof(T);  // 没有进行对齐
    const auto memory_bytes_align = round_up(memory_bytes);  // 对齐了的
    
    // 一、大内存块
    if (memory_bytes > THRESHOLD) {  
        auto rst = static_cast<T *>(malloc(memory_bytes));
        if (rst == nullptr) {
            throw exception("大内存块申请失败，内存大小：%lu", memory_bytes);
        }
        return rst;
    }

    // 二、伙伴算法申请内存
    // 自由链表仍然有剩余内存块
    array_index index = getIndex(memory_bytes);  // 计算在自由链表中的索引
    if (free_list_head[index].next != nullptr) {  // 如果有空闲块，那么直接返回一个空闲块即可
        auto rst = free_list_head[index].next;
        free_list_head[index].next = rst->next;
        return reinterpret_cast<T *>(rst);
    }
    
    // 向段内存申请 block_number/2 个内存块
    if (const auto threshold_bytes = memory_bytes_align / 2 * BLOCK_NUMBER; 
        left_memory_bytes >= threshold_bytes) {
        // 剩余内存还很多，一次性申请 BLOCK_NUMBER / 2 个内存块
        auto *new_heap = current_memory;
        insert_free_list(new_heap, memory_bytes_align, BLOCK_NUMBER / 2);
        current_memory += threshold_bytes;
        left_memory_bytes -= threshold_bytes;
        return allocate(number);
    }
    // 向段内存申请 1 个内存块
    if (left_memory_bytes >= memory_bytes_align) {
        // 剩余内存还可以申请一个内存块
        auto *new_heap = current_memory;
        insert_free_list(new_heap, memory_bytes_align, 1);
        current_memory += memory_bytes_align;
        left_memory_bytes -= memory_bytes_align;
        return allocate(number);
    }
    // 段内存资源不够了，重新申请段内存资源，并形成链表
    chunk_memory();
    return allocate(number);
}
```

## 内存回收

和内存分配反向就是了，如果大内存块直接还给系统，小内存块加入到自由链表

1.   通过计算内存大小判断是否直接还给操作系统
2.   不还给操作系统，就计算对齐内存，并加入到对应自由链表即可

## 对象构造与对象析构

```cpp
// 对象构造
template<class T>
template<class... Args>
void Allocator<T>::construct(T *start_memory, const variable_count number, Args&&... args) {
    for (variable_count i = 0; i < number; ++i) {
        new(start_memory + i) T(std::forward<Args>(args)...);
    }
}
// 对象析构
template<typename T>
void Allocator<T>::Destruct(T *start_memory, const variable_count number) {
    for (variable_count i = 0; i < number; ++i) {
        start_memory[i].~T();
    }
}
```



## 其他讲解

-   快速内存对齐，下面代码参考[ChatGPT](https://chatgpt.com/share/679a1e03-d098-8008-93d9-9c2192fe8e4a)的介绍

    >   ```cpp
    >   unsigned int align = 8; // 必须是 2 的次方，count的代码是返回一个 align 的倍数，并且大于等于 t 的一个数，内存分配器默认是分配8字节的整数倍来实现的
    >   unsigned int count(unsigned int t){
    >   return ((t + align - 1) & ~(align - 1));
    >   }
    >   // 简单理解，下面都是二进制表示的
    >   // 需要内存 t 		=    000001100111101
    >   // 对齐位置 align 	=    000000001000000 
    >   // align_1 = align-1 =  000000000111111 
    >   // t + align_1 = 		000001101111100 // 计算会超过 align 的最低位
    >   // ~(align-1)			111111111000000 // 把 t 的1后面所有位变成0,但是保留1及其前面的位
    >   // return 			=	000001101000000
    >   ```
    >


