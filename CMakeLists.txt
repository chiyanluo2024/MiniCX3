cmake_minimum_required(VERSION 3.24...3.29)
cmake_policy(SET CMP0167 OLD)

project(MiniCX3 VERSION 1.0)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(gtest_disable_pthreads ON)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v -fcolor-diagnostics -target x86_64-pc-windows-msvc -I/home/chiyan/winsdk/crt/include/ -I/home/chiyan/winsdk/sdk/include/cppwinrt/ -I/home/chiyan/winsdk/sdk/include/shared/ -I/home/chiyan/winsdk/sdk/include/ucrt/ -I/home/chiyan/winsdk/sdk/include/um/ -I/home/chiyan/winsdk/sdk/include/winrt/ -I/home/chiyan/projects/boost_1_86_0/ -flto -march=native -fuse-ld=lld -std=c++17 -Xlinker /force -L/home/chiyan/winsdk/crt/lib/x64/ -L/home/chiyan/winsdk/sdk/lib/ucrt/x64/ -L/home/chiyan/winsdk/sdk/lib/um/x64/")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # using GCC
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++
endif()

add_library(compiler_flags INTERFACE)

target_compile_features(compiler_flags INTERFACE cxx_std_17)

include(FetchContent)
option(USE_LOCAL_GOOGLETEST "Use local version of GOOGLTEST" ON)
if (USE_LOCAL_GOOGLETEST)
  FetchContent_Declare(
    googletest
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/../googletest/"
  )
else()
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG 9ff2450a56aed4f7f124f5104d9e3088bf791ee9 #release-1.15.0
    FIND_PACKAGE_ARGS NAMES GTest
  )
endif()

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(Source)
add_subdirectory(Script)
add_subdirectory(ScriptTest)


