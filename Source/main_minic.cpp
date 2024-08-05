#include "vm.hpp"
#include "compiler.hpp"
#include "function.hpp"
#include "error_handler.hpp"
#include "config.hpp"
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    char const* filename;
    if (argc > 1)
    {
        filename = argv[1];
    }
    else
    {
        std::cerr << "Error: No input file provided." << std::endl;
        return 1;
    }

    std::ifstream in(filename, std::ios_base::in);

    if (!in)
    {
        std::cerr << "Error: Could not open input file: "
            << filename << std::endl;
        return 1;
    }

    std::string source_code; // We will read the contents here.
    in.unsetf(std::ios::skipws); // No white space skipping!
    std::copy(
        std::istream_iterator<char>(in),
        std::istream_iterator<char>(),
        std::back_inserter(source_code));

    using client::parser::iterator_type;
    iterator_type iter = source_code.begin();
    iterator_type end = source_code.end();

    client::vmachine vm;                        // Our virtual machine
    client::ast::function_list ast;             // Our AST

    using boost::spirit::x3::with;
    using client::parser::error_handler_type;
    error_handler_type error_handler(iter, end, std::cerr); // Our error handler

    std::vector<int> code;
    client::code_gen::function func(code, 0);
    client::code_gen::compiler compiler(code, func, error_handler); // Our compiler

    // Our parser
    auto const parser =
        // we pass our error handler to the parser so we can access
        // it later on in our on_error and on_sucess handlers
        with<client::parser::error_handler_tag>(std::ref(error_handler))
        [
            client::function_list()
        ];

    //using boost::spirit::x3::ascii::space;
    bool success = phrase_parse(iter, end, parser, client::parser::skipper, ast);

    std::cout << "-------------------------\n";

    if (success && iter == end)
    {
        if (compiler(ast))
        {
            boost::shared_ptr<client::code_gen::function>
                p = compiler.find_function("main");
            if (!p)
                return 1;

            int nargs = argc - 2;
            if (p->nargs() != nargs)
            {
                std::cerr << "Error: main function requires " << p->nargs() << " arguments." << std::endl;
                std::cerr << nargs << "supplied." << std::endl;
                return 1;
            }

            std::cout << "Success\n";
            std::cout << "-------------------------\n";
            std::cout << "Assembler----------------\n\n";
            compiler.print_assembler();

            // Push the arguments into our stack
            for (int i = 0; i < nargs; ++i)
                vm.get_stack()[i] = boost::lexical_cast<int>(argv[i + 2]);

            // Call the interpreter
            int r = vm.execute(compiler.get_code());

            std::cout << "-------------------------\n";
            std::cout << "Result: " << r << std::endl;
            std::cout << "-------------------------\n\n";
        }
        else
        {
            std::cout << "Compile failure\n";
        }
    }
    else
    {
        std::cout << "Parse failure\n";
    }
    return 0;
}