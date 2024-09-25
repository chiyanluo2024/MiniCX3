#if !defined(COMPILER_H)
#define COMPILER_H

#include "ast.h"
#include "vm.h"
#include "error_handler.h"
#include <vector>
#include <map>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace client {
    namespace code_gen
    {
        namespace x3 = boost::spirit::x3;
        struct compiler
        {
            typedef bool result_type;
            typedef std::function<
                void(x3::position_tagged, std::string const&)>
                error_handler_type;

            template <typename ErrorHandler>
            compiler(std::vector<bytecode>& cd, std::vector<int>& idx, std::map<std::string, size_t>& var,
                const std::map<std::string, size_t>& a2d,
                std::vector<std::vector<double>>& lcl, std::vector<loopData>& ld,
                ErrorHandler const& error_handler)
                : code(cd), index(idx), variables(var), array2d(a2d), local(lcl), loopInfo(ld),
                error_handler(
                    [&](x3::position_tagged pos, std::string const& msg)
                    { error_handler(pos, msg); }
                )
            {}

            bool operator()(ast::nil) { BOOST_ASSERT(0); return false; }
            bool operator()(double x);
            bool operator()(bool x);
            bool operator()(ast::identifier const& x);
            bool operator()(ast::operation const& x);
            bool operator()(ast::unary const& x);
            bool operator()(ast::array_sequence const& x);
            bool operator()(ast::array_index const& x);
            bool operator()(ast::array2d_index const& x);
            bool operator()(ast::function_call const& x);
            bool operator()(ast::expression const& x);
            bool operator()(ast::assignment const& x);
            bool operator()(ast::array_assignment const& x);
            bool operator()(ast::array2d_assignment const& x);
            bool operator()(ast::statement const& x);
            bool operator()(ast::compound_expression const& x);
            bool operator()(ast::if_expression const& x);
            bool operator()(ast::list_expression const& x);

            void print_variables(std::vector<double> const& stack) const;
            void print_assembler() const;

            size_t const* find_var(std::string const& name) const;
            void add_var(std::string const& name);
            void add(std::string const& name, std::vector<double> const& val);
            std::vector<bytecode>& get_code() { return code; }
            std::vector<bytecode> const& get_code() const { return code; }
            std::vector<int> const& get_index() const { return index; }
            std::vector<std::vector<double> > const& get_local() const { return local; }

            typedef std::pair<bytecode, size_t> funcsig;
            typedef std::map<std::string, funcsig> funcmap;
            static const funcmap functions;

        private:

            std::vector<bytecode>& code;
            std::vector<int>& index;
            std::map<std::string, size_t>& variables;
            const std::map<std::string, size_t>& array2d;
            std::vector<std::vector<double>>& local;
            std::vector<loopData>& loopInfo;

            error_handler_type error_handler;
        };
    }
}

#endif