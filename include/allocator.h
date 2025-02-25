//
// Created by 24983 on 25-2-22.
//

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef>
#include <utility>
#include "exception.h"
#include <ostream>

namespace tinyWheels{
    template <typename  T>
    class Allocator {
        union OBJ{
            OBJ* next;
            char data[1];
        };

        using memory_size_type = size_t;     // 表示内存大小的变量类型
        using array_index = size_t;      // 表示数组索引的变量类型
        using memory_content = OBJ;     // 内存块存储的内容
        using variable_count = size_t;  // 表示变量个数的变量类型
        using block_number = size_t;      // 表示块数量的变量类型
        using null_ptr_type = void *;  // 空指针类型
        using memory_ptr_type = char *; // 地址指针类型

        constexpr static auto PTR_BYTES = sizeof(null_ptr_type);
        // 计算对齐大小，所有的块内存都应该是 ALIGN 的倍数，但是必须大于 8，因为需要存储指针
        constexpr static memory_size_type ALIGN = alignof(T) > PTR_BYTES? alignof(T) : PTR_BYTES; // 64位系统下，指针大小为 8 字节
        // 阈值，当需要分配的内存大于阈值时，从堆中获取内存块，同样也是需要看传入的类型是否太大了
        constexpr static memory_size_type THRESHOLD = 16 * PTR_BYTES < alignof(T) ? alignof(T) : 16 * PTR_BYTES;
        constexpr static block_number BLOCK_NUMBER = 20;  // 每次从堆中获取的内存块数量


        static array_index getIndex(const memory_size_type memory_bytes) {return round_up(memory_bytes) / ALIGN - 1;}  // 计算在自由链表中的索引

        static const array_index FREE_LIST_LENGTH; // = getIndex(THRESHOLD) + 1;  // 自由链表的最大索引
        // static memory_content **free_list_head;// = nullptr; // [FREE_LIST_LENGTH] = {nullptr};  // 自由链表，存储的是内存块的指针
        static memory_content *free_list_head; //


        static memory_ptr_type start_memory;    // 第一个大内存块的起始地址
        static memory_size_type left_memory_bytes;   // 当前大内存块中剩余的内存大小
        static memory_ptr_type current_memory;  // 当前可申请的内存块起始地址


        // 申请一大块内存，内存大小为：chunk_memory_bytes字节，并且加入到之前的链表中
        static void chunk_memory() {
            // 申请大内存块，一个大内存块由 chunk_memory_bytes 字节组成，并在最后存有一个指针，指向下一个大内存块
            // 等差数列通项公式：a_n = n * ALIGN * BLOCK_NUM，求前 FREE_LIST_LENGTH 项和
            const auto chunk_memory_bytes = (1 + FREE_LIST_LENGTH) * FREE_LIST_LENGTH / 2 * ALIGN * BLOCK_NUMBER;
            const auto malloc_memory_address = static_cast<memory_ptr_type>(malloc(chunk_memory_bytes + PTR_BYTES));  // 申请的内存首地址

            auto next_chunk_address = reinterpret_cast<memory_ptr_type *>(malloc_memory_address + chunk_memory_bytes);
            *next_chunk_address = nullptr;  // 下一个大内存块的地址
            if (start_memory == nullptr) {  // 如果地址为空，直接把申请的地址给它
                start_memory = malloc_memory_address;
            }else {  // 否则，把申请的地址加入到链表中
                next_chunk_address = reinterpret_cast<memory_ptr_type *>(start_memory + chunk_memory_bytes);
                *next_chunk_address = malloc_memory_address;
            }
            // 当前段内存资源还有剩余内存可以使用，从小到大依次分配内存块
            // 计算剩余内存分配策略，从 left_memory_bytes / 2 开始，每次分配一半的内存
            const auto cur_memory_unit = round_up(left_memory_bytes / 2);  // 分配当前内存块大小，一定是 ALIGN 的倍数，且一定小于等于 left_memory_bytes，并且一定在 free_list_head 中

            for (auto i = getIndex(cur_memory_unit); left_memory_bytes > 0; i = i > 0 ? i-1: 0) {
                const auto memory_unit_bytes = (i + 1) * ALIGN;
                if (left_memory_bytes < memory_unit_bytes) {
                    continue;
                }
                insert_free_list(malloc_memory_address + i * ALIGN, memory_unit_bytes, 1, true);
            }
            current_memory = malloc_memory_address;  // 更新当前可申请的内存块起始地址
            left_memory_bytes = chunk_memory_bytes;  // 更新剩余内存大小
        }

        // 给定一大段内存资源的起始地址，单个内存块大小，内存块的数量，将这一大块内存资源初始化成自由链表，并加入到自由链表头节点中
        static void insert_free_list(const memory_ptr_type start_address, const memory_size_type memory_unit_bytes, const block_number block, auto update = true) {
            if (start_address == nullptr or memory_unit_bytes == 0 or block == 0) {
                return;
            }
            auto index = memory_unit_bytes / ALIGN - 1;
            auto& fl = free_list_head[index];
            auto last = fl.next;
            auto prev = &fl;
            for (block_number i = 0; i < block; ++i, prev = prev->next) {
                auto cur = reinterpret_cast<memory_content *>(start_address + i * memory_unit_bytes);
                *cur = memory_content{last};
                prev->next = cur;
            }
            if (update) {
                current_memory += block * memory_unit_bytes;
                left_memory_bytes -= block * memory_unit_bytes;
            }
        }

        static void log(int index = -1) {
            if (out_point != nullptr) {
                if (free_list_head == nullptr) {
                    *out_point << "自由链表未初始化" << std::endl;
                    return;
                }
                if (index < 0) {
                    for (array_index i = 0; i < FREE_LIST_LENGTH; ++i) {
                        *out_point << "free_list[" << i << "] = ";
                        auto fl = &free_list_head[i];
                        while (fl != nullptr) {
                            *out_point << reinterpret_cast<void *>(fl) << " -> ";
                            fl = fl->next;
                        }
                        *out_point << "nullptr" << std::endl;
                    }
                }else {
                    auto i = static_cast<array_index>(index);
                    *out_point << "free_list[" << i << "] = ";
                    auto fl = &free_list_head[i];
                    while (fl != nullptr) {
                        *out_point << reinterpret_cast<void *>(fl) << " -> ";
                        fl = fl->next;
                    }
                    *out_point << "nullptr" << std::endl;
                }
                *out_point << "=========================" << std::endl;
            }
        }
        static std::ostream* out_point;  // 输出流指针
    public:
        static void set_log_ostream(std::ostream& os) {
            out_point = &os;
        }

        static void destroy_all() {  // 销毁所有的内存块
            if (start_memory != nullptr) {
                const auto chunk_memory_bytes = (1 + FREE_LIST_LENGTH) * FREE_LIST_LENGTH / 2 * ALIGN;
                // auto next_chunk_address = reinterpret_cast<memory_ptr_type *>(start_memory + chunk_memory_bytes);
                auto next_chunk_address = reinterpret_cast<memory_ptr_type *>(start_memory);
                while (next_chunk_address) {
                    const auto tmp = next_chunk_address;
                    next_chunk_address = next_chunk_address + chunk_memory_bytes;
                    free(tmp);
                }
                start_memory = nullptr;
                left_memory_bytes = 0;
                current_memory = nullptr;
            }
        }

        static memory_size_type round_up(memory_size_type memory_bytes) {
            memory_bytes = memory_bytes == 0 ? 1 : memory_bytes;
            return ((memory_bytes + ALIGN - 1) & ~(ALIGN - 1));
        }  // 计算最小需要获取的字节并向上取 ALIGN 的倍数整
        // 内存分配：分配大小为 memory_bytes 的内存块，通过计算 number * sizeof(T) 来获取内存块大小，返回地址以及实际容量
        // static T *allocate(variable_count  number = 1);
        static std::pair<T *, memory_size_type> allocate(variable_count number);
        // 内存释放：释放内存块，释放大小为 number 的内存块，起始地址为 start_memory
        static void deallocate(T *start_memory, variable_count number);
        // 对象构造：在起始地址为 start_memory 的内存块上构造 number 个对象
        template<class... Args>
        static void construct(T *start_memory, variable_count number, Args&&... args);
        template<class... Args>
        static void construct(T *start_memory, variable_count number, const Args&... args);
        static void construct(T *start_memory, variable_count number);
        // 对象析构：在起始地址为 start_memory 的内存块上析构 number 个对象
        static void Destruct(T *start_memory, variable_count number);



        Allocator() = delete;
        Allocator(const Allocator&) = delete;
        Allocator(Allocator&&) = delete;
        Allocator& operator=(const Allocator&) = delete;
        Allocator& operator=(Allocator&&) = delete;
        ~Allocator() = default;
    };

    // 计算free list的长度
    template <class T>
    const typename Allocator<T>::array_index Allocator<T>::FREE_LIST_LENGTH = getIndex(THRESHOLD) + 1;
    // 初始化free list指向nullptr
    // template <class T>
    // typename Allocator<T>::memory_content **Allocator<T>::free_list_head = nullptr;
    template <class T>
    typename Allocator<T>::memory_content *Allocator<T>::free_list_head = nullptr;
    // 初始化起始地址，并让其指向nullptr
    template <class T>
    typename Allocator<T>::memory_ptr_type Allocator<T>::start_memory = nullptr;
    // 初始化剩余内存大小
    template <class T>
    typename Allocator<T>::memory_size_type Allocator<T>::left_memory_bytes = 0;
    // 初始化当前可申请的内存块起始地址
    template <class T>
    typename Allocator<T>::memory_ptr_type Allocator<T>::current_memory = nullptr;

    template <class T>
    std::ostream* Allocator<T>::out_point = nullptr;

    // 内存分配
    template <class T>
    std::pair<T *, typename Allocator<T>::memory_size_type>Allocator<T>::allocate(const variable_count number) {
        // 判断自由链表是否初始化成功
        if (free_list_head == nullptr) {
            const auto length = FREE_LIST_LENGTH;
            free_list_head = static_cast<memory_content *>(malloc(length * sizeof(memory_content)));
            if (free_list_head == nullptr) {
                throw exception("自由链表初始化失败，内存大小：%lu", length * sizeof(memory_content));
            }
            for (array_index i = 0; i < length; ++i) {
                *(free_list_head + i) = memory_content{nullptr};
            }
        }

        // 计算需要的内存
        const memory_size_type memory_bytes = number * sizeof(T);  // 没有进行对齐
        // 两种方法：1. 从自由链表中获取内存块 2. 从堆中获取内存块
        // 计算总共内存，如果总共内存大于阈值，则从堆中获取内存块，否则从自由链表中获取内存块
        if (memory_bytes > THRESHOLD) {  // 方式 2，大内存块
            auto rst = static_cast<T *>(malloc(memory_bytes));
            if (rst == nullptr) {
                throw exception("大内存块申请失败，内存大小：%lu", memory_bytes);
            }
            return std::make_pair(rst, memory_bytes / sizeof(T));
        }

        // 下面是方式1，小内存块，如果自由链表中仍然有空闲块，那么直接返回一个空闲块即可
        const auto memory_bytes_align = round_up(memory_bytes);  // 需要申请的单个内存块大小
        array_index index = getIndex(memory_bytes);  // 计算在自由链表中的索引
        if (free_list_head[index].next != nullptr) {  // 如果有空闲块，那么直接返回一个空闲块即可
            auto rst = free_list_head[index].next;
            free_list_head[index].next = rst->next;
            if (memory_bytes_align == 56)
                log(static_cast<int>(index));
            return std::make_pair(reinterpret_cast<T *>(rst), memory_bytes_align / sizeof(T));
        }

        // 下面是方式1，小内存块，如果自由链表中没有空闲块，那么就申请大内存块中的空闲块
        // - 1. 计算需要的内存大小
        // 否则如果有一个空闲块，那么返回一个空闲块，在否则，调用chunk_memory()函数，申请一大块内存，然后返回几个BLOCK_NUMBER / 2个空闲块
        // 从堆中获取内存块
        if (const auto threshold_bytes = memory_bytes_align / 2 * BLOCK_NUMBER; left_memory_bytes >= threshold_bytes) {
            // 剩余内存还很多，一次性申请 BLOCK_NUMBER / 2 个内存块
            auto *new_heap = current_memory;
            insert_free_list(new_heap, memory_bytes_align, BLOCK_NUMBER / 2, true);
            return allocate(number);
        }
        if (left_memory_bytes >= memory_bytes_align) {
            // 剩余内存还可以申请一个内存块
            auto *new_heap = current_memory;
            insert_free_list(new_heap, memory_bytes_align, 1, true);
            return allocate(number);
        }
        chunk_memory();
        return allocate(number);
    }

    // 内存释放
    template <class T>
    void Allocator<T>::deallocate(T* start_memory, const variable_count number) {
        if (free_list_head == nullptr) {
            throw exception("请先开始分配内存再释放内存，自由链表未成功初始化");
        }

        const memory_size_type memory_bytes = number * sizeof(T);  // 没有进行对齐

        if (memory_bytes > THRESHOLD) {
            free(start_memory);
            return;
        }
        // 2. 如果内存大小小于阈值，那么就采用自由链表的方式释放内存
        const auto memory_bytes_align = round_up(memory_bytes);
        insert_free_list(reinterpret_cast<memory_ptr_type>(start_memory), memory_bytes_align, 1, false);
    }


    // 对象构造
    template<class T>
    template<class... Args>
    void Allocator<T>::construct(T *start_memory, const variable_count number, Args&&... args) {
        for (variable_count i = 0; i < number; ++i) {
            new(start_memory + i) T(std::forward<Args>(args)...);
        }
    }
    template<typename T>
    template<class... Args>
    void Allocator<T>::construct(T *start_memory, variable_count number, const Args &... args) {
        for (variable_count i = 0; i < number; ++i) {
            new(start_memory + i) T(std::forward<Args>(args)...);
        }
    }
    template<typename T>
    void Allocator<T>::construct(T *start_memory, variable_count number) {
        for (variable_count i = 0; i < number; ++i) {
            new(start_memory + i) T();
        }
    }

    // 对象析构
    template<typename T>
    void Allocator<T>::Destruct(T *start_memory, const variable_count number) {
        for (variable_count i = 0; i < number; ++i) {
            start_memory[i].~T();
        }
    }
}

#endif //ALLOCATOR_H
