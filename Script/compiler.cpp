#include "compiler.h"
#include "vm.h"
#include <boost/foreach.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <set>
#include <iostream>

#if defined(operator)
#undef operator
#endif

namespace client {
    namespace code_gen
    {
        /*static*/ const compiler::funcmap compiler::functions = {
        { {"plus",1}, funcsig(op_floor, 1)},
        { {"minus",1}, funcsig(op_cap, 1)},
        { {"zero",1}, funcsig(op_zero, 1)},
        { {"abs",1}, funcsig(op_abs, 1)},
        { {"sqr",1}, funcsig(op_sqr, 1)},
        { {"sqrt",1}, funcsig(op_sqrt, 1)},
        { {"exp",1}, funcsig(op_exp, 1)},
        { {"log",1}, funcsig(op_log, 1)},
        { {"sum",1}, funcsig(op_sum, 1)},
        { {"prod",1}, funcsig(op_prod, 1)},
        { {"min",1}, funcsig(op_min, 1)},
        { {"max",1}, funcsig(op_max, 1)},
        { {"max",2}, funcsig(op_emax, 2)},
        { {"min",2}, funcsig(op_emin, 2)},
        { {"interp",3}, funcsig(op_interp, 3)},
        };

        size_t const* compiler::find_var(std::string const& name) const
        {
            std::map<std::string, size_t>::const_iterator i = variables.find(name);
            if (i == variables.end())
                return 0;
            return &i->second;
        }

        void compiler::add_var(std::string const& name)
        {
            std::size_t n = variables.size();
            variables[name] = n;
        }

        void compiler::add(std::string const& name, std::vector<double> const& val) {
            add_var(name);
            local.push_back(val);
        }

        bool compiler::operator()(bool x)
        {
            code.push_back(x ? op_true : op_false);
            return true;
        }

        bool compiler::operator()(double x)
        {
            code.push_back(op_push);
            local.push_back(std::vector<double>(1, x));
            return true;
        }

        bool compiler::operator()(ast::identifier const& x)
        {
            size_t const* p = find_var(x.name);
            if (p == 0)
            {
                error_handler(x, "Undeclared variable: " + x.name);
                return false;
            }
            code.push_back(op_load);
            index.push_back(int(*p));
            return true;
        }

        bool compiler::operator()(ast::operation const& x)
        {
            if (!boost::apply_visitor(*this, x.operand_))
                return false;
            switch (x.operator_)
            {
            case ast::op_append: code.push_back(op_append); break;
            case ast::op_plus: code.push_back(op_add); break;
            case ast::op_minus: code.push_back(op_sub); break;
            case ast::op_times: code.push_back(op_mul); break;
            case ast::op_divide: code.push_back(op_div); break;
            case ast::op_power: code.push_back(op_pow); break;
            case ast::op_equal: code.push_back(op_eq); break;
            case ast::op_not_equal: code.push_back(op_neq); break;
            case ast::op_less: code.push_back(op_lt); break;
            case ast::op_less_equal: code.push_back(op_lte); break;
            case ast::op_greater: code.push_back(op_gt); break;
            case ast::op_greater_equal: code.push_back(op_gte); break;
            case ast::op_and: code.push_back(op_and); break;
            case ast::op_or: code.push_back(op_or); break;
            default: BOOST_ASSERT(0); return false;
            }
            return true;
        }

        bool compiler::operator()(ast::unary const& x)
        {
            if (!boost::apply_visitor(*this, x.operand_))
                return false;
            switch (x.operator_)
            {
            case ast::op_not: code.push_back(op_not); break;
            case ast::op_positive: break;
            case ast::op_negative: code.push_back(op_neg); break;
            case ast::op_len: code.push_back(op_len); break;
            default: BOOST_ASSERT(0); return false;
            }
            return true;
        }

        bool compiler::operator()(ast::function_call const& x)
        {
            auto psig = functions.find({ x.function_name.name, x.args.size() });
            if (psig == functions.end())
            {
                error_handler(x.function_name, "Function not found: " + x.function_name.name + " with " + std::to_string(x.args.size()) + " args");
                return false;
            }

            BOOST_FOREACH(ast::expression const& expr, x.args)
            {
                if (!(*this)(expr))
                    return false;
            }

            auto p = psig->second;
            code.push_back(p.first);
            index.push_back(int(p.second));

            return true;
        }

        bool compiler::operator()(ast::array_sequence const& x)
        {
            if (!boost::apply_visitor(*this, x.start))
                return false;

            int i = 0;
            BOOST_FOREACH(ast::operand const& oper, x.rest)
            {
                ++i;
                if (i > 2) {
                    error_handler(ast::identifier(), "More than 2 operands are found within the use of : which is not supported");
                    return false;
                }
                if (!boost::apply_visitor(*this, oper))
                    return false;
            }
            if (i) {
                code.push_back(op_seq);
                index.push_back(i);
            }
            return true;
        }

        bool compiler::operator()(ast::array_index const& x)
        {
            size_t const* p = find_var(x.array_name.name);
            if (p == 0)
            {
                error_handler(x.array_name, "Undeclared array: " + x.array_name.name);
                return false;
            }
            if (!(*this)(x.index))
                return false;

            code.push_back(op_index);
            index.push_back(int(*p));

            return true;
        }

        bool compiler::operator()(ast::array2d_index const& x)
        {
            size_t const* p = find_var(x.array_name.name);
            if (p == 0)
            {
                error_handler(x.array_name, "Undeclared array2d: " + x.array_name.name);
                return false;
            }
            std::map<std::string, size_t>::const_iterator i = array2d.find(x.array_name.name);
            size_t dim = (i != array2d.end()) ? i->second : 0;

            if (!(*this)(x.index1))
                return false;

            code.push_back(op_push);
            local.push_back(std::vector<double>(1, double(dim)));
            code.push_back(op_mul);

            if (!(*this)(x.index2))
                return false;

            code.push_back(op_add);
            code.push_back(op_index);
            index.push_back(int(*p));

            return true;
        }

        bool compiler::operator()(ast::expression const& x)
        {
            if (!boost::apply_visitor(*this, x.first))
                return false;
            BOOST_FOREACH(ast::operation const& oper, x.rest)
            {
                if (!(*this)(oper))
                    return false;
            }
            return true;
        }

        bool compiler::operator()(ast::assignment const& x)
        {
            if (!(*this)(x.rhs))
                return false;
            size_t const* p = find_var(x.lhs.name);
            if (p == 0)
            {
                add_var(x.lhs.name);
                p = find_var(x.lhs.name);
                //error_handler(x.lhs, "Undeclared variable: " + x.lhs.name);
                //return false;
            }
            code.push_back(op_store);
            index.push_back(int(*p));
            return true;
        }

        bool compiler::operator()(ast::array_assignment const& x)
        {
            if (!(*this)(x.rhs))
                return false;

            size_t const* p = find_var(x.array_name.name);
            if (p == 0)
            {
                error_handler(x.array_name, "Undeclared array: " + x.array_name.name);
                return false;
            }
            if (!(*this)(x.index))
                return false;

            code.push_back(op_indexstore);
            index.push_back(int(*p));
            return true;
        }

        bool compiler::operator()(ast::array2d_assignment const& x)
        {
            if (!(*this)(x.rhs))
                return false;

            size_t const* p = find_var(x.array_name.name);
            if (p == 0)
            {
                error_handler(x.array_name, "Undeclared array2d: " + x.array_name.name);
                return false;
            }
            std::map<std::string, size_t>::const_iterator i = array2d.find(x.array_name.name);
            size_t dim = (i != array2d.end()) ? i->second : 0;

            if (!(*this)(x.index1))
                return false;

            code.push_back(op_push);
            local.push_back(std::vector<double>(1, double(dim)));
            code.push_back(op_mul);

            if (!(*this)(x.index2))
                return false;

            code.push_back(op_add);
            code.push_back(op_indexstore);
            index.push_back(int(*p));
            return true;
        }

        bool compiler::operator()(ast::statement const& x)
        {
            return boost::apply_visitor(*this, x);
        }

        bool compiler::operator()(ast::compound_expression const& x)
        {
            BOOST_FOREACH(ast::statement const& s, x.list)
            {
                if (!(*this)(s))
                    return false;
            }
            return (*this)(x.main);
        }

        bool compiler::operator()(ast::if_expression const& x)
        {
            if (!(*this)(x.condition))
                return false;
            code.push_back(op_jump_if);
            size_t ic = code.size();
            size_t iv = local.size();
            size_t ii = index.size();
            index.insert(index.end(), { 0, 0, 0 });     // placeholder to be updated later
            if (!(*this)(x.then))
                return false;
            code.push_back(op_jump);
            size_t ic2 = code.size();
            size_t iv2 = local.size();
            size_t ii2 = index.size();
            index.insert(index.end(), { 0, 0, 0 }); // placeholder to be updated
            index[ii] = int(code.size() - ic);
            index[ii + 1] = int(local.size() - iv);
            index[ii + 2] = int(index.size() - ii);
            if (!(*this)(x.else_))
                return false;
            index[ii2] = int(code.size() - ic2);
            index[ii2 + 1] = int(local.size() - iv2);
            index[ii2 + 2] = int(index.size() - ii2);
 
            return true;
        }

        bool compiler::operator()(ast::list_expression const& x)
        {
            size_t const* p = find_var(x.var.name);
            if (p == 0)
            {
                add_var(x.var.name);
                p = find_var(x.var.name);
            }
            if (!(*this)(x.range))
                return false;
            code.push_back(op_loop_start);
            int iloop = int(loopInfo.size());
            index.push_back(iloop);
            loopInfo.push_back(loopData({ *p, {}, 0, 0 }));
            size_t ic = code.size();
            size_t iv = local.size();
            size_t ii = index.size();
            index.insert(index.end(), { 0, 0, 0 });     // placeholder to be updated later
            if (!(*this)(x.body))
                return false;
            code.push_back(op_loop_end);
            index.push_back(iloop);
            int jc = int(code.size() - ic);
            int jv = int(local.size() - iv);
            int ji = int(index.size() - ii);
            index.insert(index.end(), { -jc, -jv, -ji + 3 });
            index[ii] = jc;
            index[ii + 1] = jv;
            index[ii + 2] = ji;
            return true;
        }

        void compiler::print_variables(std::vector<double> const& stack) const
        {
            for (auto const& p : variables)
            {
                std::cout << "    " << p.first << ": " << stack[p.second] << std::endl;
            }
        }

        void compiler::print_assembler() const
        {
            auto pc = code.begin();
            auto pi = index.begin();
            auto pv = local.begin();

            std::vector<std::string> locals(variables.size());
            typedef std::pair<std::string, size_t> pair;
            for (pair const& p : variables)
            {
                locals[p.second] = p.first;
                std::cout << "local       "
                    << p.first << ", @" << p.second << std::endl;
            }

            std::map<std::size_t, std::string> lines;
            std::set<std::size_t> jumps;

            while (pc != code.begin())
            {
                std::string line;
                std::size_t address = pc - code.begin();

                switch (*pc++)
                {
                case op_neg:
                    line += "      op_neg";
                    break;

                case op_not:
                    line += "      op_not";
                    break;

                case op_len:
                    line += "      op_len";
                    break;

                case op_sum:
                    line += "      op_sum";
                    break;

                case op_prod:
                    line += "      op_prod";
                    break;

                case op_min:
                    line += "      op_min";
                    break;

                case op_max:
                    line += "      op_max";
                    break;

                case op_append:
                    line += "      op_append";
                    break;

                case op_add:
                    line += "      op_add";
                    break;

                case op_sub:
                    line += "      op_sub";
                    break;

                case op_mul:
                    line += "      op_mul";
                    break;

                case op_div:
                    line += "      op_div";
                    break;

                case op_pow:
                    line += "      op_pow";
                    break;

                case op_eq:
                    line += "      op_eq";
                    break;

                case op_neq:
                    line += "      op_neq";
                    break;

                case op_lt:
                    line += "      op_lt";
                    break;

                case op_lte:
                    line += "      op_lte";
                    break;

                case op_gt:
                    line += "      op_gt";
                    break;

                case op_gte:
                    line += "      op_gte";
                    break;

                case op_emax:
                    line += "      op_emax";
                    break;

                case op_emin:
                    line += "      op_emin";
                    break;

                case op_and:
                    line += "      op_and";
                    break;

                case op_or:
                    line += "      op_or";
                    break;

                case op_load:
                    line += "      op_load     ";
                    line += locals[*pc++];
                    break;

                case op_store:
                    line += "      op_store    ";
                    line += locals[*pc++];
                    break;

                case op_index:
                    line += "      op_index    ";
                    break;

                case op_indexstore:
                    line += "      op_indexstore";
                    line += *pi++;
                    break;

                case op_push:
                    line += "      op_push      ";
                    for (size_t i = 0; i < (*pv).size(); ++i) {
                        line += boost::lexical_cast<std::string>((*pv)[i]);
                        if (i + 1 < (*pv).size()) {
                            line += ",";
                        }
                    }
                    ++pv;
                    break;

                case op_true:
                    line += "      op_true";
                    break;

                case op_false:
                    line += "      op_false";
                    break;

                case op_jump:
                {
                    line += "      op_jump     ";
                    std::size_t pos = (pc - code.begin()) + *pc++;
                    if (pos == code.size())
                        line += "end";
                    else
                        line += std::to_string(pos);
                    jumps.insert(pos);
                }
                break;

                case op_jump_if:
                {
                    line += "      op_jump_if  ";
                    std::size_t pos = (pc - code.begin()) + *pc++;
                    if (pos == code.size())
                        line += "end";
                    else
                        line += std::to_string(pos);
                    jumps.insert(pos);
                }
                break;

                case op_floor:
                    line += "      op_floor";
                    break;

                case op_cap:
                    line += "      op_cap";
                    break;

                case op_zero:
                    line += "      op_zero";
                    break;

                case op_abs:
                    line += "      op_abs";
                    break;

                case op_sqr:
                    line += "      op_sqr";
                    break;

                case op_sqrt:
                    line += "      op_sqrt";
                    break;

                case op_exp:
                    line += "      op_exp";
                    break;

                case op_log:
                    line += "      op_log";
                    break;

                case op_interp:
                    line += "	   op_interp";
                    break;

                    lines[address] = line;
                }

                std::cout << "start:" << std::endl;
                for (auto const& l : lines)
                {
                    std::size_t pos = l.first;
                    if (jumps.find(pos) != jumps.end())
                        std::cout << pos << ':' << std::endl;
                    std::cout << l.second << std::endl;
                }

                std::cout << "end:" << std::endl;
            }
        }
    }
}