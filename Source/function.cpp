#include "function_def.hpp"
#include "config.hpp"

namespace client {
    namespace parser
    {
        BOOST_SPIRIT_INSTANTIATE(function_type, iterator_type, context_type);
        BOOST_SPIRIT_INSTANTIATE(function_list_type, iterator_type, context_type);
    }
}