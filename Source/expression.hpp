#if !defined(BOOST_SPIRIT_X3_MINIC_EXPRESSION_HPP)
#define BOOST_SPIRIT_X3_MINIC_EXPRESSION_HPP

#include <boost/spirit/home/x3.hpp>
#include "ast.hpp"

namespace client
{
    namespace x3 = boost::spirit::x3;
    namespace parser
    {
        struct expression_class;
        typedef x3::rule<expression_class, ast::expression> expression_type;

        BOOST_SPIRIT_DECLARE(expression_type);
    }

    parser::expression_type const& expression();
    //parser::skipper_type const& skipper();
}

#endif