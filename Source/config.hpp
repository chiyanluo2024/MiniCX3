#if !defined(BOOST_SPIRIT_X3_MINIC_CONFIG_HPP)
#define BOOST_SPIRIT_X3_MINIC_CONFIG_HPP

#include <boost/spirit/home/x3.hpp>
#include "error_handler.hpp"

namespace client {
    namespace parser
    {
        using skipper_type = decltype(skipper);

        typedef std::string::const_iterator iterator_type;
        typedef x3::phrase_parse_context<skipper_type>::type phrase_context_type;
        typedef error_handler<iterator_type> error_handler_type;

        typedef x3::context<
            error_handler_tag
            , std::reference_wrapper<error_handler_type>
            , phrase_context_type>
            context_type;
    }
}

#endif