# Tiny Wheels with CPP

# 项目背景

刚好在找实习+学校的开源激励计划参加了但是一直没有什么好的idea或者进展，就打算用这个项目来一举两得吧。

# 前言

所以决定复刻一个`STL(Standard Template Library)`和一些`tinyWebserver`等组件。

本项目将采用`Modern cpp`来实现，coding时处理使用以下几个工具并 **未做其他测试**：
- C++20
- Linux gcc 编译器: gcc (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
- cmake 编译工具: version 3.28.3
- Linux: `Ubuntu 24.04.2 LTS` for WSL2
- Clion IDE Remote Development

重要参考项目：
- [标准模板库](https://github.com/gcc-mirror/gcc/tree/master/libstdc%2B%2B-v3/include/bits)
- [TinySTL](https://github.com/zouxiaohang/TinySTL)

# 开发文档

开发过程中的文档在`doc`文件夹中，如果是`0-*.md`文件，表示基础知识，其他的才是实现文档

# 项目结构及介绍

项目的实现并没有严格的顺序，而是根据学习进度来的，但是有些细节部分还是会有顺序的，比如需要先实现内存池的功能，才能实现`STL`的其他组件。


```
├── CMakeLists.txt 	// cmake 构建文件
├── include			// 头文件。如果是 *.impl.h 文件，相当于 .cpp 文件，这是因为模板编程的特殊性，如果是 *.def.h 文件，相当于 .h 文件
│   ├── *.h			// 所有头文件
├── LICENSE			// 开源许可证
├── doc			    // 实现项目的所有文档，注意是实现，不是使用文档
├── main.cpp		// 整体测试文件
├── README.md		// 
├── src  			// 库的实现，源文件，目前没有写在这里面。
├── test			// 单元测试文件
│   ├── test_*.cpp	// 所有单元测试文件
├── bin
│   ├── test
│   |   ├── test_*	// 单元测试文件可执行文件
|   ├── main			// 整体测试可执行文件
```

# 注意事项

1. 由于本项目实现的`STL`并没有经过非常**严格**的测试，所以在实现其他组件的时候，可能还是会先会采用`std`的库，以保证程序的**正确性**。
2. 本项目的目的不是为了取代任何一个市面上的库。
