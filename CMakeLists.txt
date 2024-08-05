cmake_minimum_required(VERSION 3.24...3.29)

project(MiniCX3 VERSION 1.0)

if (MSVC)
  add_compile_options(/std:c++17)
endif()
add_library(compiler_flags INTERFACE)

target_compile_features(compiler_flags INTERFACE cxx_std_17)

add_subdirectory(Source)


