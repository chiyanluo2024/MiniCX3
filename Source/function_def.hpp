#if !defined(BOOST_SPIRIT_X3_MINIC_FUNCTION_DEF_HPP)
#define BOOST_SPIRIT_X3_MINIC_FUNCTION_DEF_HPP

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>
#include "ast.hpp"
#include "ast_adapted.hpp"
#include "statement.hpp"
#include "function.hpp"
#include "common.hpp"
#include "error_handler.hpp"

namespace client {
    namespace parser
    {
        using x3::raw;
        using x3::lexeme;
        using x3::char_;
        using x3::alnum;
        using namespace x3::ascii;

        struct name_class;

        typedef x3::rule<name_class, ast::identifier> name_type;

        function_type const function("function");
        function_list_type const function_list("function_list");
        name_type const name("name");

        auto const function_def =
            lexeme[((char_('i') > char_('n') > char_('t')) | (char_('v') > char_('o') > char_('i') > char_('d'))) >> !(alnum | '_')]
            > name
            > '(' > -(name % ',') > ')'
            > '{' > statement() > '}'
            ;
        auto const function_list_def = +function;
        auto const name_def = identifier;

        BOOST_SPIRIT_DEFINE(
            function
            , function_list
            , name
        );

        struct function_class : error_handler_base {};
        struct function_name : x3::annotate_on_success {};
    }
}

namespace client
{
    parser::function_type const& function()
    {
        return parser::function;
    }

    parser::function_list_type const& function_list()
    {
        return parser::function_list;
    }
}

#endif