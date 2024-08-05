#if !defined(BOOST_SPIRIT_MINIC_COMPILER_HPP)
#define BOOST_SPIRIT_MINIC_COMPILER_HPP

#include "ast.hpp"
#include "error_handler.hpp"
#include <vector>
#include <map>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace client {
    namespace code_gen
    {
        ///////////////////////////////////////////////////////////////////////////
        //  The Function
        ///////////////////////////////////////////////////////////////////////////
        struct function
        {
            function(std::vector<int>& code, int nargs)
                : code(code), address(code.size()), size_(0), nargs_(nargs) {}

            void op(int a);
            void op(int a, int b);
            void op(int a, int b, int c);

            int& operator[](std::size_t i) { return code[address + i]; }
            int const& operator[](std::size_t i) const { return code[address + i]; }
            void clear() { code.clear(); variables.clear(); }
            std::size_t size() const { return size_; }
            std::size_t get_address() const { return address; }

            size_t nargs() const { return nargs_; }
            size_t nvars() const { return variables.size(); }
            int const* find_var(std::string const& name) const;
            void add_var(std::string const& name);
            void link_to(std::string const& name, std::size_t address);

            void print_variables(std::vector<int> const& stack) const;
            void print_assembler() const;

        private:

            std::map<std::string, int> variables;
            std::map<std::size_t, std::string> function_calls;
            std::vector<int>& code;
            std::size_t address;
            std::size_t size_;
            std::size_t nargs_;
        };

        ///////////////////////////////////////////////////////////////////////////
        //  The Compiler
        ///////////////////////////////////////////////////////////////////////////
        struct compiler
        {
            typedef bool result_type;
            typedef std::function<
                void(x3::position_tagged, std::string const&)>
                error_handler_type;

            template <typename ErrorHandler>
            compiler(std::vector<int>& cd, code_gen::function& f, ErrorHandler const& error_handler)
                : code(cd), current(&f), error_handler(
                    [&](x3::position_tagged pos, std::string const& msg)
                    { error_handler(pos, msg); }
                )
            {}

            bool operator()(ast::nil) { BOOST_ASSERT(0); return false; }
            bool operator()(unsigned int x);
            bool operator()(bool x);
            bool operator()(ast::identifier const& x);
            bool operator()(ast::operation const& x);
            bool operator()(ast::unary const& x);
            bool operator()(ast::function_call const& x);
            bool operator()(ast::expression const& x);
            bool operator()(ast::assignment const& x);
            bool operator()(ast::variable_declaration const& x);
            bool operator()(ast::statement_list const& x);
            bool operator()(ast::statement const& x);
            bool operator()(ast::if_statement const& x);
            bool operator()(ast::while_statement const& x);
            bool operator()(ast::return_statement const& x);
            bool operator()(ast::function const& x);
            bool operator()(ast::function_list const& x);

            bool start(ast::statement_list const& x);

            void print_variables(std::vector<int> const& stack) const;
            void print_assembler() const;

            boost::shared_ptr<code_gen::function>
                find_function(std::string const& name) const;

            std::vector<int>& get_code() { return code; }
            std::vector<int> const& get_code() const { return code; }

        private:

            typedef std::map<std::string, boost::shared_ptr<code_gen::function> > function_table;

            std::vector<int>& code;
            code_gen::function* current;
            std::string current_function_name;
            function_table functions;
            bool void_return;

            error_handler_type error_handler;
        };
    }
}

#endif