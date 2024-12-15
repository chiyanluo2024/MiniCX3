#if !defined(EXPRESSION_H)
#define EXPRESSION_H

#include <boost/spirit/home/x3.hpp>
#include "ast.h"

namespace client
{
    namespace x3 = boost::spirit::x3;
    namespace parser
    {
        struct compound_expression_class;
        typedef x3::rule<compound_expression_class, ast::compound_expression> compound_expression_type;
        BOOST_SPIRIT_DECLARE(compound_expression_type);

        struct expression_block_class;
        typedef x3::rule<expression_block_class, ast::expression_block> expression_block_type;
        BOOST_SPIRIT_DECLARE(expression_block_type);
    }

    parser::compound_expression_type const& compound_expression();
    parser::expression_block_type const& expression_block();
}

#endif