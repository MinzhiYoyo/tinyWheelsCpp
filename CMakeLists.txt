cmake_minimum_required(VERSION 3.28)
project(tinyWheelCpp)
# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(core_dir ${PROJECT_SOURCE_DIR}/wheels)
set(binary_dir ${PROJECT_SOURCE_DIR}/bin)
set(test_dir ${PROJECT_SOURCE_DIR}/test)
set(library_dir ${PROJECT_SOURCE_DIR}/lib)
set(include_dir ${PROJECT_SOURCE_DIR}/include)

file(GLOB CPP_FILES "main.cpp")

# 获取 core 下所有的*.cpp文件
file(GLOB_RECURSE CORE_FILES "${core_dir}/*.cpp")

# 获取 test 下所有的测试文件
file(GLOB_RECURSE TEST_FILES "${test_dir}/test_*.cpp")

# 将测试文件添加到CPP_FILES中
list(APPEND CPP_FILES ${TEST_FILES})

# 自动创建bin目录
add_custom_target(create_bin_dir ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${binary_dir}
)
# 自动创建lib目录
add_custom_target(create_lib_dir ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${library_dir}
)

# 单独编译每个静态库
foreach (core_file ${CORE_FILES})
    # 获取文件相对于项目根目录的路径
    file(RELATIVE_PATH rel_path ${PROJECT_SOURCE_DIR} ${core_file})
    # 获取输出目录结构（去除文件名部分）
    get_filename_component(output_dir "${rel_path}" DIRECTORY)
    get_filename_component(basename "${rel_path}" NAME_WE)
    string(REPLACE ".cpp" "" basename "${basename}")

    # 创建静态库
    add_library(${basename} STATIC ${core_file})
    # 设置输出路径
    set_target_properties(${basename} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${library_dir}/${output_dir}"
    )
    target_include_directories(${basename} PRIVATE ${core_dir})
endforeach ()

# 获取所有的 .a 静态库
file(GLOB_RECURSE LIB_FILES "${library_dir}/*.a")

# 打印所有的静态库，逐行打印
message(STATUS "可执行文件用到的静态库如下：")
foreach (lib_file ${LIB_FILES})
    message(STATUS ${lib_file})
endforeach ()


# 编译测试文件及主文件
foreach (cpp_file ${CPP_FILES})
    # 获取文件相对于项目根目录的路径
    file(RELATIVE_PATH rel_path ${PROJECT_SOURCE_DIR} ${cpp_file})
    # 获取输出目录结构（去除文件名部分）
    get_filename_component(output_dir "${rel_path}" DIRECTORY)
    get_filename_component(basename "${rel_path}" NAME_WE)
    string(REPLACE ".cpp" "" basename "${basename}")

    # 创建可执行目标，需要把所有的源文件链接进去
    add_executable(${basename} ${cpp_file} ${CORE_FILES})

    # 设置输出路径
    set_target_properties(${basename} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${binary_dir}/${output_dir}"
    )
    # 添加静态库
    target_link_libraries(${basename} ${LIB_FILES})
    target_include_directories(${basename} PRIVATE ${core_dir} ${include_dir})
    target_link_libraries(${basename} mysqlclient)

endforeach ()

