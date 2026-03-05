cmake_minimum_required(VERSION 3.22)
project(aui_examples)

set(AUI_VERSION v8.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC)
    add_compile_options(/utf-8)
    add_compile_definitions(_UTF8_SOURCE)
endif()
# import AUI
# auib_import(aui https://github.com/aui-framework/aui
#         COMPONENTS core views audio
#         VERSION ${AUI_VERSION})

find_package(aui REQUIRED)

add_subdirectory(ATreeView)
add_subdirectory(TreeView)
