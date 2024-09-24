#if !defined(AST_H)
#define AST_H

#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/optional.hpp>
#include <list>

namespace client {
    namespace ast
    {
        ///////////////////////////////////////////////////////////////////////////
        //  The AST
        ///////////////////////////////////////////////////////////////////////////
        namespace x3 = boost::spirit::x3;

        struct nil {};
        struct unary;
        struct expression;
        struct array_sequence;
        struct array_index;
        struct array2d_index;
        struct function_call;
        struct statement;
        struct compound_expression;
        struct if_expression;
        struct list_expression;

        struct identifier : x3::position_tagged
        {
            identifier(std::string const& name = "") : name(name) {}
            std::string name;
        };

        struct operand :
            x3::variant<
            nil
            , bool
            , double
            , identifier
            , x3::forward_ast<unary>
            , x3::forward_ast<array_sequence>
            , x3::forward_ast<array_index>
            , x3::forward_ast<array2d_index>
            , x3::forward_ast<function_call>
            , x3::forward_ast<expression>
            , x3::forward_ast<compound_expression>
            , x3::forward_ast<if_expression>
            , x3::forward_ast<list_expression>
            >
        {
            using base_type::base_type;
            using base_type::operator=;
        };

        enum optoken
        {
            op_not,
            op_positive,
            op_negative,
            op_len,
            op_append,
            op_plus,
            op_minus,
            op_times,
            op_divide,
            op_power,
            op_equal,
            op_not_equal,
            op_less,
            op_less_equal,
            op_greater,
            op_greater_equal,
            op_and,
            op_or
        };

        struct unary
        {
            optoken operator_;
            operand operand_;
        };

        struct operation : x3::position_tagged
        {
            optoken operator_;
            operand operand_;
        };

        struct expression : x3::position_tagged
        {
            operand first;
            std::list<operation> rest;
        };

        struct array_sequence
        {
            operand start;
            std::list<operand> rest;
        };

        struct array_index : x3::position_tagged
        {
            identifier array_name;
            expression index;
        };

        struct array2d_index : x3::position_tagged
        {
            identifier array_name;
            expression index1;
            expression index2;
        };

        struct function_call : x3::position_tagged
        {
            identifier function_name;
            std::list<expression> args;
        };

        struct assignment : x3::position_tagged
        {
            identifier lhs;
            expression rhs;
        };

        struct array_assignment : x3::position_tagged
        {
            identifier array_name;
            expression index;
            expression rhs;
        };

        struct array2d_assignment : x3::position_tagged
        {
            identifier array_name;
            expression index1;
            expression index2;
            expression rhs;
        };

        struct statement :
            x3::variant<
            assignment
            , array_assignment
            , array2d_assignment
            , expression
            >
        {
            using base_type::base_type;
            using base_type::operator=;
        };

        struct compound_expression : x3::position_tagged
        {
            std::list<statement> list;
            expression main;
        };

        struct if_expression
        {
            expression condition;
            compound_expression then;
            compound_expression else_;
        };

        struct list_expression
        {
            identifier var;
            expression range;
            compound_expression body;
        };

        // print functions for debugging
        inline std::ostream& operator<<(std::ostream& out, nil)
        {
            out << std::string("nil"); return out;
        }

        inline std::ostream& operator<<(std::ostream& out, identifier const& id)
        {
            out << id.name; return out;
        }
    }
}

#endif