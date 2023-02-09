cmake_minimum_required(VERSION 3.6)


project(winterfell C CXX)

add_compile_options(
 -g
 -std=c++14
 # -DVALGRIND
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 -Werror
 -Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 # -MMD
 -rdynamic
)


include_directories(${PROJECT_SOURCE_DIR}) # include 默认路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(DCMAKE_VERBOSE_MAKEFILE ON) # 打印makefile 编译命令


add_subdirectory(winterfell/base)
add_subdirectory(winterfell/util)
add_subdirectory(winterfell/net)
add_subdirectory(example)