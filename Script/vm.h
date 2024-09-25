#if !defined(VM_H)
#define VM_H

#include <vector>
#include <map>
#include <string>

namespace client
{
    enum bytecode
    {
        // unary (element wise)
        op_neg,         //  negate the top stack entry
        op_not,         //  boolean negate the top stack entry
        op_floor,		//	floor the top stack entry at 0
        op_cap,			//	cap the top stack entry at 0
        op_zero,		//	returns a zero vector for given size in the top stack entry
        op_abs,			//	returns the absolute value of the top stack entry
        op_sqr,			//	returns the square of the top stack entry
        op_sqrt,		//	returns the square root of the top stack entry
        op_exp,			//	returns the exponential of the top stack entry
        op_log,			//	returns the natural logarithm of the top stack entry

        // unary (scalar)
        op_len,			//  array length of the top stack entry
        op_sum,			//  sum of the components of the top stack entry
        op_prod,		//  product of the components of the top stack entry
        op_min,			//	min of the components of the top stack entry
        op_max,			//	max of the components of the top stack entry

        // binary
        op_append,		//  append top two stack entries as array
        op_add,         //  add top two stack entries
        op_sub,         //  subtract top two stack entries
        op_mul,         //  multiply top two stack entries
        op_div,         //  divide top two stack entries
        op_pow,			//  compute the power of the top two stack entries
        op_eq,          //  compare the top two stack entries for ==
        op_neq,         //  compare the top two stack entries for !=
        op_lt,          //  compare the top two stack entries for <
        op_lte,         //  compare the top two stack entries for <=
        op_gt,          //  compare the top two stack entries for >
        op_gte,         //  compare the top two stack entries for >=
        op_emax,		//  take the element-wise max of the top two stack entries
        op_emin,		//  take the element-wise min of the top two stack entries
        op_and,         //  logical and top two stack entries
        op_or,          //  logical or top two stack entries

        // ternary
        op_interp,		//	interp(xx,x,y) returns the interpolation of y on a grid of x at the input values of xx

        // others
        op_load,        //  load a variable on local to stack
        op_store,       //  store a variable on stack to local
        op_seq,			//	create a sequence using the top stack entries (in the format of start:end or start:end:step)
        op_index,		//  index local variable at top stack entry
        op_indexstore,  //  store a variable on stack to local array index
        op_push,		//  push constant into the stack
        op_true,        //  push constant 0 into the stack
        op_false,       //  push constant 1 into the stack

        op_jump_if,     //  jump to a relative position in the code if top stack evaluates to false
        op_jump,        //  jump to a relative position in the code
        op_loop_start,  //  loop start for a body of code over a variable on stack
        op_loop_end,    //  loop end for a body of code over a variable on stack
    };

    struct loopData {
        size_t iVar = 0;
        std::vector<double> range;
        size_t iCurr = 0;
        size_t resSize = 0;
    };

    class vmachine
    {
    public:

        static const double dbl_eps;
        vmachine(std::vector<double>& st,
            std::vector<size_t>& sz,
            std::vector<std::vector<double>>& lcl)
            : stack(st), size(sz), local(lcl)
        {
        }

        void execute(const std::vector<bytecode>& code,
            const std::vector<int>& index,
            const std::map<std::string, size_t>& variable,
            const std::vector<std::vector<double>>& data,
            std::vector<loopData>& loopInfo,
            size_t ninput);

        std::vector<double> const& get_stack() const { return stack; }
        std::vector<double>& get_stack() { return stack; }
        std::vector<std::vector<double>>& get_local() { return local; }
        void get_stack_top(std::vector<double>& val) const;

    private:
        // covers: op_neg, op_plus, op_minus, op_not, all element-wise unary functions
        void proc_op1_(bytecode c, std::vector<double>::iterator& iter);

        // covers: op_add, op_sub, op_mul, op_div, op_pow, op_eq, op_neq, op_lt, op_lte, op_gt, op_gte, op_and, op_or
        void proc_op2_(bytecode c, std::vector<double>::iterator& iter1, std::vector<double>::iterator& iter2, const double* tmp);

        void add(const std::vector<double>& v, std::vector<double>::iterator& stack_ptr, std::vector<size_t>::iterator& size_ptr);

        std::vector<double>& stack;
        std::vector<size_t>& size;
        std::vector<std::vector<double>>& local;

        std::vector<double>::iterator stack_ptr;
        std::vector<size_t>::iterator size_ptr;
    };
}

#endif