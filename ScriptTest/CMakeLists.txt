set(ScriptTest_SOURCE pch.h test.cpp)

enable_testing()

add_executable(ScriptTest ${ScriptTest_SOURCE})

target_link_libraries(ScriptTest PRIVATE Script compiler_flags GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(ScriptTest)