#if !defined(BOOST_SPIRIT_X3_MINIC_AST_ADAPTED_HPP)
#define BOOST_SPIRIT_X3_MINIC_AST_ADAPTED_HPP

#include "ast.hpp"
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(client::ast::unary,
    operator_, operand_
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::operation,
    operator_, operand_
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::function_call,
    function_name, args
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::expression,
    first, rest
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::variable_declaration,
    lhs, rhs
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::assignment,
    lhs, rhs
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::return_statement,
    expr
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::if_statement,
    condition, then, else_
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::while_statement,
    condition, body
)

BOOST_FUSION_ADAPT_STRUCT(client::ast::function,
    return_type, function_name, args, body
)

#endif
