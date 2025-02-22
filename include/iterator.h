//
// Created by 24983 on 25-2-22.
//

#ifndef ITERATOR_H
#define ITERATOR_H
#include <cstddef>

namespace tinyWheels{
        // 五种迭代器类型tag
    struct input_iterator_tag{};
    struct output_iterator_tag{};
    struct forward_iterator_tag : input_iterator_tag {};
    struct bidirectional_iterator_tag : forward_iterator_tag {};
    struct random_access_iterator_tag : bidirectional_iterator_tag {};

    // 五种迭代器类型
    template <class T, class Distance>
    struct input_iterator
    {
        using iterator_category = input_iterator_tag;             // 迭代器类型
        using value_type = T;                                    // 迭代器所指对象的类型
        using difference_type = Distance;                       // 两个迭代器之间的距离
        using pointer = T*;                                     // 迭代器所指对象的指针
        using reference = T&;                                   // 迭代器所指对象的引用
    };
    struct output_iterator
    {
        using iterator_category = output_iterator_tag;           // 迭代器类型
        using value_type = void;                                // 迭代器所指对象的类型
        using difference_type = void;                           // 两个迭代器之间的距离
        using pointer = void;                                   // 迭代器所指对象的指针
        using reference = void;                                 // 迭代器所指对象的引用
    };
    template <class T, class Distance>
    struct forward_iterator
    {
        using iterator_category = forward_iterator_tag;          // 迭代器类型
        using value_type = T;                                    // 迭代器所指对象的类型
        using difference_type = Distance;                       // 两个迭代器之间的距离
        using pointer = T*;                                     // 迭代器所指对象的指针
        using reference = T&;                                   // 迭代器所指对象的引用
    };
    template <class T, class Distance>
    struct bidirectional_iterator
    {
        using iterator_category = bidirectional_iterator_tag;    // 迭代器类型
        using value_type = T;                                    // 迭代器所指对象的类型
        using difference_type = Distance;                       // 两个迭代器之间的距离
        using pointer = T*;                                     // 迭代器所指对象的指针
        using reference = T&;                                   // 迭代器所指对象的引用
    };
    template <class T, class Distance>
    struct random_access_iterator
    {
        using iterator_category = random_access_iterator_tag;    // 迭代器类型
        using value_type = T;                                    // 迭代器所指对象的类型
        using difference_type = Distance;                       // 两个迭代器之间的距离
        using pointer = T*;                                     // 迭代器所指对象的指针
        using reference = T&;                                   // 迭代器所指对象的引用
    };


    // 实际的迭代器模板
    template<class Category, class T, class Distance = ptrdiff_t,
    class Pointer = T*, class Reference = T&>
    struct iterator
    {
        using iterator_category = Category;             // 迭代器类型
        using value_type = T;                           // 迭代器所指对象的类型
        using difference_type = Distance;               // 两个迭代器之间的距离
        using pointer = Pointer;                        // 迭代器所指对象的指针
        using reference = Reference;                    // 迭代器所指对象的引用
    };

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


    // // 萃取迭代器类型
    // template<class Iterator>
    // inline typename iterator_traits<Iterator>::iterator_category
    // iterator_category(const Iterator&)
    // {
    //     using category = typename iterator_traits<Iterator>::iterator_category;
    //     return category();  // 返回迭代器类型，是一个结构体对象
    // }
    // // 萃取迭代器所指对象的类型
    // template<class Iterator>
    // inline typename iterator_traits<Iterator>::value_type*
    // value_type(const Iterator&)
    // {
    //     return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    // }
    // // 萃取迭代器之间的距离
    // template<class Iterator>
    // inline typename iterator_traits<Iterator>::difference_type*
    // difference_type(const Iterator&)
    // {
    //     return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    // }
}

#endif //ITERATOR_H
