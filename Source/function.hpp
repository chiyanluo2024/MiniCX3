#if !defined(BOOST_SPIRIT_X3_MINIC_FUNCTION_HPP)
#define BOOST_SPIRIT_X3_MINIC_FUNCTION_HPP

#include <boost/spirit/home/x3.hpp>
#include "ast.hpp"

namespace client
{
    namespace x3 = boost::spirit::x3;
    namespace parser
    {
        struct function_class;
        struct function_list_class;
        typedef x3::rule<function_class, ast::function> function_type;
        typedef x3::rule<function_list_class, ast::function_list> function_list_type;
        typedef function_type::id function_id;
        typedef function_list_type::id function_list_id;
        BOOST_SPIRIT_DECLARE(function_type, function_list_type);
    }

    parser::function_type const& function();
    parser::function_list_type const& function_list();
}

#endif