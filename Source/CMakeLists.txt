set(HEADER ast.hpp ast_adapted.hpp common.hpp compiler.hpp config.hpp error_handler.hpp expression.hpp expression_def.hpp function.hpp function_def.hpp statement.hpp statement_def.hpp vm.hpp)
set(SOURCE compiler.cpp expression.cpp function.cpp statement.cpp vm.cpp)

FIND_PACKAGE( Boost 1.85 COMPONENTS program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_executable(MinicX3 ${SOURCE} main_minic.cpp)
add_executable(Calc9X3 ${SOURCE} main_calc9.cpp)

target_link_libraries(MinicX3 PRIVATE compiler_flags)
target_link_libraries(Calc9X3 PRIVATE compiler_flags)
