/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_MINIC_STATEMENT_DEF_HPP)
#define BOOST_SPIRIT_X3_MINIC_STATEMENT_DEF_HPP

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>
#include "ast.hpp"
#include "ast_adapted.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "common.hpp"
#include "error_handler.hpp"

namespace client {
    namespace parser
    {
        using x3::raw;
        using x3::lexeme;
        using namespace x3::ascii;

        struct _statement_class;
        struct statement_list_class;
        struct variable_declaration_class;
        struct assignment_class;
        struct return_statement_class;
        struct if_statement_class;
        struct while_statement_class;
        struct compound_statement_class;

        typedef x3::rule<_statement_class, ast::statement> _statement_type;
        typedef x3::rule<statement_list_class, ast::statement_list> statement_list_type;
        typedef x3::rule<variable_declaration_class, ast::variable_declaration> variable_declaration_type;
        typedef x3::rule<assignment_class, ast::assignment> assignment_type;
        typedef x3::rule<return_statement_class, ast::return_statement> return_statement_type;
        typedef x3::rule<if_statement_class, ast::if_statement> if_statement_type;
        typedef x3::rule<while_statement_class, ast::while_statement> while_statement_type;
        typedef x3::rule<compound_statement_class, ast::statement_list> compound_statement_type;

        statement_type const statement("statement");
        _statement_type const _statement("_statement");
        statement_list_type const statement_list("statement_list");
        variable_declaration_type const variable_declaration("variable_declaration");
        assignment_type const assignment("assignment");
        return_statement_type const return_statement("return_statement");
        if_statement_type const if_statement("if_statement");
        while_statement_type const while_statement("while_statement");
        compound_statement_type const compound_statement("compound_statement");

        // Import the expression rule
        namespace { auto const& expression = client::expression(); }

        auto const statement_def = statement_list;

        auto const statement_list_def =
            +_statement
            ;

        auto const _statement_def = 
            variable_declaration
            | assignment
            | return_statement
            | if_statement
            | while_statement
            | compound_statement
            ;

        auto const variable_declaration_def =
            lexeme["int" >> !(alnum | '_')] // make sure we have whole words
        > identifier
        > -('=' > expression)
        > ';'
            ;

        auto const assignment_def =
            identifier
        > '='
        > expression
        > ';'
            ;

        auto const return_statement_def =
            lexeme["return" >> !(alnum | '_')] // make sure we have whole words
            > -expression
            > ';'
            ;

        auto const if_statement_def =
            lit("if")
            > '('
            > expression
            > ')'
            > _statement
            >
            -(
                lexeme["else" >> !(alnum | '_')] // make sure we have whole words
                > _statement
                )
            ;

        auto const while_statement_def =
            lit("while")
            > '('
            > expression
            > ')'
            > _statement
            ;

        auto const compound_statement_def =
            '{' >> -statement_list >> '}'
            ;

        BOOST_SPIRIT_DEFINE(
            statement
            , _statement
            , statement_list
            , variable_declaration
            , assignment
            , return_statement
            , if_statement
            , while_statement
            , compound_statement
        );

        struct statement_list_class : error_handler_base {};
        struct variable_declaration_class : x3::annotate_on_success {};
        struct assignment_class : x3::annotate_on_success {};
        struct return_statement_class : x3::annotate_on_success {};
    }
}

namespace client
{
    parser::statement_type const& statement()
    {
        return parser::statement;
    }
}

#endif