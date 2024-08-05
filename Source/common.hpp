#if !defined(BOOST_SPIRIT_X3_MINIC_COMMON_HPP)
#define BOOST_SPIRIT_X3_MINIC_COMMON_HPP

#include <boost/spirit/home/x3.hpp>

namespace client {
    namespace parser
    {
        using x3::raw;
        using x3::lexeme;
        using x3::alpha;
        using x3::alnum;

        struct identifier_class;
        typedef x3::rule<identifier_class, std::string> identifier_type;
        static identifier_type const identifier = "identifier";
        static x3::symbols<> const keywords({ "true", "false", "if", "else", "while", "int", "void", "return" });

        auto const identifier_def = !keywords >> raw[lexeme[(alpha | '_') >> *(alnum | '_')]];

        BOOST_SPIRIT_DEFINE(identifier);
    }
}

#endif