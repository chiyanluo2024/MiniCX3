#if !defined(AST_ADAPTED_H)
#define AST_ADAPTED_H

#include "ast.h"
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(client::ast::unary,
    operator_, operand_
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::operation,
    operator_, operand_
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::expression,
    first, rest
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::array_sequence,
    start, rest
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::array_index,
    array_name, index
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::array2d_index,
    array_name, index1, index2
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::function_call,
    function_name, args
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::assignment,
    lhs, rhs
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::array_assignment,
    array_name, index, rhs
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::array2d_assignment,
    array_name, index1, index2, rhs
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::compound_expression,
    list, main
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::if_expression,
    condition, then, else_
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::list_expression,
    var, range, body
)

#endif
