cmake_minimum_required(VERSION 3.24...3.29)

project(MiniCX3 VERSION 1.0)

if (MSVC)
  add_compile_options(/std:c++17)
endif()
add_library(compiler_flags INTERFACE)

target_compile_features(compiler_flags INTERFACE cxx_std_17)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG 9ff2450a56aed4f7f124f5104d9e3088bf791ee9 #release-1.15.0
  FIND_PACKAGE_ARGS NAMES GTest
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(Source)
add_subdirectory(Script)
add_subdirectory(ScriptTest)


