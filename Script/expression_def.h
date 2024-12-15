#if !defined(EXPRESSION_DEF_H)
#define EXPRESSION_DEF_H

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>
#include "ast.h"
#include "ast_adapted.h"
#include "expression.h"
#include "common.h"
#include "error_handler.h"

namespace client {
    namespace parser
    {
        using x3::double_;
        using x3::char_;
        using x3::bool_;
        using x3::raw;
        using x3::lexeme;
        using namespace x3::ascii;

        ////////////////////////////////////////////////////////////////////////////
        // Tokens
        ////////////////////////////////////////////////////////////////////////////

        x3::symbols<ast::optoken> concat_op;
        x3::symbols<ast::optoken> logical_or_op;
        x3::symbols<ast::optoken> logical_and_op;
        x3::symbols<ast::optoken> equality_op;
        x3::symbols<ast::optoken> relational_op;
        x3::symbols<ast::optoken> additive_op;
        x3::symbols<ast::optoken> multiplicative_op;
        x3::symbols<ast::optoken> power_op;
        x3::symbols<ast::optoken> unary_op;

        void add_keywords()
        {
            static bool once = false;
            if (once)
                return;
            once = true;

            concat_op.add
            ("&", ast::op_append)
                ;

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

            power_op.add
            ("^", ast::op_power)
                ;

            unary_op.add
            ("+", ast::op_positive)
                ("-", ast::op_negative)
                ("!", ast::op_not)
                ("#", ast::op_len)
                ;

            keywords.add
                ("true")
                ("false")
                ("if")
                ("list")
                ;
        }

        ////////////////////////////////////////////////////////////////////////////
        // Main expression grammar
        ////////////////////////////////////////////////////////////////////////////

        struct declname_class;
        struct expression_class;
        struct concat_expr_class;
        struct logical_or_expr_class;
        struct logical_and_expr_class;
        struct equality_expr_class;
        struct relational_expr_class;
        struct additive_expr_class;
        struct multiplicative_expr_class;
        struct power_expr_class;
        struct unary_expr_class;
        struct primary_expr_class;
        struct array_sequence_class;
        struct array_index_class;
        struct array2d_index_class;
        struct function_call_class;

        struct statement_class;
        struct assignment_class;
        struct array_assignment_class;
        struct array2d_assignment_class;
        struct if_expression_class;
        struct list_expression_class;

        typedef x3::rule<declname_class, ast::identifier> declname_type;
        typedef x3::rule<expression_class, ast::expression> expression_type;
        typedef x3::rule<concat_expr_class, ast::expression> concat_expr_type;
        typedef x3::rule<logical_or_expr_class, ast::expression> logical_or_expr_type;
        typedef x3::rule<logical_and_expr_class, ast::expression> logical_and_expr_type;
        typedef x3::rule<equality_expr_class, ast::expression> equality_expr_type;
        typedef x3::rule<relational_expr_class, ast::expression> relational_expr_type;
        typedef x3::rule<additive_expr_class, ast::expression> additive_expr_type;
        typedef x3::rule<multiplicative_expr_class, ast::expression> multiplicative_expr_type;
        typedef x3::rule<power_expr_class, ast::expression> power_expr_type;
        typedef x3::rule<unary_expr_class, ast::operand> unary_expr_type;
        typedef x3::rule<primary_expr_class, ast::operand> primary_expr_type;
        typedef x3::rule<array_sequence_class, ast::array_sequence> array_sequence_type;
        typedef x3::rule<array_index_class, ast::array_index> array_index_type;
        typedef x3::rule<array2d_index_class, ast::array2d_index> array2d_index_type;
        typedef x3::rule<function_call_class, ast::function_call> function_call_type;

        typedef x3::rule<statement_class, ast::statement> statement_type;
        typedef x3::rule<assignment_class, ast::assignment> assignment_type;
        typedef x3::rule<array_assignment_class, ast::array_assignment> array_assignment_type;
        typedef x3::rule<array2d_assignment_class, ast::array2d_assignment> array2d_assignment_type;
        typedef x3::rule<if_expression_class, ast::if_expression> if_expression_type;
        typedef x3::rule<list_expression_class, ast::list_expression> list_expression_type;

        expression_type const expression = "expression";
        declname_type const declname = "declname";
        concat_expr_type const concat_expr = "concat_expr";
        logical_or_expr_type const logical_or_expr = "logical_or_expr";
        logical_and_expr_type const logical_and_expr = "logical_and_expr";
        equality_expr_type const equality_expr = "equality_expr";
        relational_expr_type const relational_expr = "relational_expr";
        additive_expr_type const additive_expr = "additive_expr";
        multiplicative_expr_type const multiplicative_expr = "multiplicative_expr";
        power_expr_type const power_expr = "power_expr";
        unary_expr_type const unary_expr = "unary_expr";
        primary_expr_type const primary_expr = "primary_expr";
        array_sequence_type const array_sequence = "array_sequence";
        array_index_type const array_index = "array_index";
        array2d_index_type const array2d_index = "array2d_index";
        function_call_type const function_call = "function_call";

        compound_expression_type const compound_expression("compound_expression");
        statement_type const statement("statement");
        assignment_type const assignment("assignment");
        array_assignment_type const array_assignment("array_assignment");
        array2d_assignment_type const array2d_assignment("array2d_assignment");
        if_expression_type const if_expression("if_expression");
        list_expression_type const list_expression("list_expression");
        expression_block_type const expression_block("expression_block");

        auto const declname_def = identifier;

        auto const expression_def = concat_expr;

        auto const concat_expr_def =
            logical_or_expr
            >> *(concat_op > logical_or_expr)
            ;

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
            array_sequence
            >> *(relational_op > array_sequence)
            ;

        auto const array_sequence_def =
            additive_expr
            >> *(':' > additive_expr)
            ;

        auto const additive_expr_def =
            multiplicative_expr
            >> *(additive_op > multiplicative_expr)
            ;

        auto const multiplicative_expr_def =
            power_expr
            >> *(multiplicative_op > power_expr)
            ;

        auto const power_expr_def =
            unary_expr
            >> *(power_op > unary_expr)
            ;

        auto const unary_expr_def =
            primary_expr
            | (unary_op > primary_expr)
            ;

        auto const primary_expr_def =
            bool_
            | double_
            | function_call
            | array2d_index
            | array_index
            | declname
            | '(' > expression > ')'
            | if_expression
            | list_expression
            ; // order is very important as the parsing will stop at the first success

        auto const array_index_def =
            (declname >> '[')
            > expression
            > ']'
            ;

        auto const array2d_index_def =
            (declname >> '[')
            >> expression
            >> ',' 
            >> expression
            >> ']'
            ;

        auto const function_call_def =
            (declname >> '(')
            > -(expression % ',')
            > ')'
            ;

        auto const statement_def =
            array2d_assignment
            | array_assignment
            | assignment
            | (expression >> ';')
            ;

        auto const compound_expression_def =
            *statement
            > expression
            ;

        auto const assignment_def =
            ('$' > declname)
            > '='
            > expression
            > ';'
            ;

        auto const array_assignment_def = 
            ('$' > declname)
            >> '[' 
            >> expression
            > ']' 
            > '='
            > expression
            > ';'
            ;

        auto const array2d_assignment_def =
            ('$' > declname)
            >> '['
            >> expression
            >> ','
            >> expression
            >> ']'
            > '='
            > expression
            > ';'
            ;

        auto const if_expression_def =
            lit("if")
            > '('
            > expression
            > ','
            > compound_expression
            > ','
            > compound_expression
            > ')'
            ;

        auto const list_expression_def =
            lit("list")
            > '('
            > ('$' > declname)
            > ','
            > expression
            > ','
            > compound_expression
            > ')'
            ;

        auto const expression_block_def =
            lit("@") 
            > declname
            > '[' > declname > ']'
            > -('<' > declname > '>')
            > '{' > compound_expression > '}'
            ;

        BOOST_SPIRIT_DEFINE(
            expression
            , declname
            , concat_expr
            , logical_or_expr
            , logical_and_expr
            , equality_expr
            , relational_expr
            , additive_expr
            , multiplicative_expr
            , power_expr
            , unary_expr
            , primary_expr
            , array_sequence
            , array_index
            , array2d_index
            , function_call
            , statement
            , compound_expression
            , assignment
            , array_assignment
            , array2d_assignment
            , if_expression
            , list_expression
            , expression_block
        );

        struct expression_class : error_handler_base {};
        struct primary_expr_class : x3::annotate_on_success {};
        struct statement_class : x3::annotate_on_success {};
        struct compound_exression_class : error_handler_base {};
        struct assignment_class : x3::annotate_on_success {};
        struct array_assignment_class : x3::annotate_on_success {};
    }
}

namespace client
{
    parser::compound_expression_type const& compound_expression()
    {
        parser::add_keywords();
        return parser::compound_expression;
    }

    parser::expression_block_type const& expression_block()
    {
        parser::add_keywords();
        return parser::expression_block;
    }
}

#endif