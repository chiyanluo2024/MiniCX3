#if !defined(BOOST_SPIRIT_X3_MINIC_EXPRESSION_DEF_HPP)
#define BOOST_SPIRIT_X3_MINIC_EXPRESSION_DEF_HPP

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>
#include "ast.hpp"
#include "ast_adapted.hpp"
#include "expression.hpp"
#include "common.hpp"
#include "error_handler.hpp"

namespace client {
    namespace parser
    {
        using x3::uint_;
        using x3::char_;
        using x3::bool_;
        using x3::raw;
        using x3::lexeme;
        using namespace x3::ascii;

        ////////////////////////////////////////////////////////////////////////////
        // Tokens
        ////////////////////////////////////////////////////////////////////////////

        x3::symbols<ast::optoken> logical_or_op;
        x3::symbols<ast::optoken> logical_and_op;
        x3::symbols<ast::optoken> equality_op;
        x3::symbols<ast::optoken> relational_op;
        x3::symbols<ast::optoken> additive_op;
        x3::symbols<ast::optoken> multiplicative_op;
        x3::symbols<ast::optoken> unary_op;

        void add_keywords()
        {
            static bool once = false;
            if (once)
                return;
            once = true;

            logical_or_op.add
            ("||", ast::op_or)
                ;

            logical_and_op.add
            ("&&", ast::op_and)
                ;

            equality_op.add
            ("==", ast::op_equal)
                ("!=", ast::op_not_equal)
                ;

            relational_op.add
            ("<", ast::op_less)
                ("<=", ast::op_less_equal)
                (">", ast::op_greater)
                (">=", ast::op_greater_equal)
                ;

            additive_op.add
            ("+", ast::op_plus)
                ("-", ast::op_minus)
                ;

            multiplicative_op.add
            ("*", ast::op_times)
                ("/", ast::op_divide)
                ;

            unary_op.add
            ("+", ast::op_positive)
                ("-", ast::op_negative)
                ("!", ast::op_not)
                ;
        }

        ////////////////////////////////////////////////////////////////////////////
        // Main expression grammar
        ////////////////////////////////////////////////////////////////////////////

        struct logical_or_expr_class;
        struct logical_and_expr_class;
        struct equality_expr_class;
        struct relational_expr_class;
        struct additive_expr_class;
        struct multiplicative_expr_class;
        struct unary_expr_class;
        struct primary_expr_class;
        struct function_call_class;
        struct argument_list_class;

        typedef x3::rule<logical_or_expr_class, ast::expression> logical_or_expr_type;
        typedef x3::rule<logical_and_expr_class, ast::expression> logical_and_expr_type;
        typedef x3::rule<equality_expr_class, ast::expression> equality_expr_type;
        typedef x3::rule<relational_expr_class, ast::expression> relational_expr_type;
        typedef x3::rule<additive_expr_class, ast::expression> additive_expr_type;
        typedef x3::rule<multiplicative_expr_class, ast::expression> multiplicative_expr_type;
        typedef x3::rule<unary_expr_class, ast::operand> unary_expr_type;
        typedef x3::rule<primary_expr_class, ast::operand> primary_expr_type;
        typedef x3::rule<function_call_class, ast::function_call> function_call_type;
        typedef x3::rule<argument_list_class, std::list<ast::expression>> argument_list_type;

        expression_type const expression = "expression";
        logical_or_expr_type const logical_or_expr = "logical_or_expr";
        logical_and_expr_type const logical_and_expr = "logical_and_expr";
        equality_expr_type const equality_expr = "equality_expr";
        relational_expr_type const relational_expr = "relational_expr";
        additive_expr_type const additive_expr = "additive_expr";
        multiplicative_expr_type const multiplicative_expr = "multiplicative_expr";
        unary_expr_type const unary_expr = "unary_expr";
        primary_expr_type const primary_expr = "primary_expr";
        function_call_type const function_call = "function_call";
        argument_list_type const argument_list = "argument_list";

        auto const expression_def = logical_or_expr;

        auto const logical_or_expr_def =
            logical_and_expr
            >> *(logical_or_op > logical_and_expr)
            ;

        auto const logical_and_expr_def =
            equality_expr
            >> *(logical_and_op > equality_expr)
            ;

        auto const equality_expr_def =
            relational_expr
            >> *(equality_op > relational_expr)
            ;

        auto const relational_expr_def =
            additive_expr
            >> *(relational_op > additive_expr)
            ;

        auto const additive_expr_def =
            multiplicative_expr
            >> *(additive_op > multiplicative_expr)
            ;

        auto const multiplicative_expr_def =
            unary_expr
            >> *(multiplicative_op > unary_expr)
            ;

        auto const unary_expr_def =
            primary_expr
            | (unary_op > primary_expr)
            ;

        auto const primary_expr_def =
            uint_
            | function_call
            | identifier
            | bool_
            | '(' > expression > ')'
            ;

        auto const function_call_def =
            (identifier >> '(')
            > argument_list
            > ')'
            ;

        auto const argument_list_def = -(expression % ',');

        BOOST_SPIRIT_DEFINE(
            expression
            , logical_or_expr
            , logical_and_expr
            , equality_expr
            , relational_expr
            , additive_expr
            , multiplicative_expr
            , unary_expr
            , primary_expr
            , function_call
            , argument_list
        );

        struct expression_class : error_handler_base {};
        struct primary_expr_class : x3::annotate_on_success {};

    }
}

namespace client
{
    parser::expression_type const& expression()
    {
        parser::add_keywords();
        return parser::expression;
    }
}

#endif