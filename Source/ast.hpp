#if !defined(BOOST_SPIRIT_X3_MINIC_AST_HPP)
#define BOOST_SPIRIT_X3_MINIC_AST_HPP

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
        struct function_call;
        struct expression;

        struct identifier : x3::position_tagged
        {
            identifier(std::string const& name = "") : name(name) {}
            std::string name;
        };

        struct operand :
            x3::variant<
            nil
            , bool
            , unsigned int
            , identifier
            , x3::forward_ast<unary>
            , x3::forward_ast<function_call>
            , x3::forward_ast<expression>
            >
        {
            using base_type::base_type;
            using base_type::operator=;
        };

        enum optoken
        {
            op_plus,
            op_minus,
            op_times,
            op_divide,
            op_positive,
            op_negative,
            op_not,
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

        struct function_call : x3::position_tagged
        {
            identifier function_name;
            std::list<expression> args;
        };

        struct expression : x3::position_tagged
        {
            operand first;
            std::list<operation> rest;
        };

        struct variable_declaration
        {
            identifier lhs;
            boost::optional<expression> rhs;
        };

        struct assignment : x3::position_tagged
        {
            identifier lhs;
            expression rhs;
        };

        struct return_statement : x3::position_tagged
        {
            boost::optional<expression> expr;
        };

        struct if_statement;
        struct while_statement;
        struct statement_list;

        struct statement :
            x3::variant<
            variable_declaration
            , assignment
            , return_statement
            , boost::recursive_wrapper<if_statement>
            , boost::recursive_wrapper<while_statement>
        //    , boost::recursive_wrapper<return_statement>
            , boost::recursive_wrapper<statement_list>
            >
        {
            using base_type::base_type;
            using base_type::operator=;
        };

        struct if_statement
        {
            expression condition;
            statement then;
            boost::optional<statement> else_;
        };

        struct while_statement
        {
            expression condition;
            statement body;
        };

        struct statement_list : std::list<statement> {};

        struct function
        {
            std::string return_type;
            identifier function_name;
            std::list<identifier> args;
            statement_list body;
        };

        typedef std::list<function> function_list;

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