set(HEADER ast.h ast_adapted.h common.h compiler.h config.h error_handler.h expression.h expression_def.h ScriptInterface.h vm.h Normal.h)
set(SOURCE compiler.cpp expression.cpp ScriptInterface.cpp vm.cpp Normal.cpp)

FIND_PACKAGE( Boost 1.85 COMPONENTS program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_library(Script ${SOURCE})

target_include_directories(Script INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Script PRIVATE compiler_flags)
