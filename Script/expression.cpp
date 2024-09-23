#include "expression_def.h"
#include "config.h"

namespace client {
    namespace parser
    {
        BOOST_SPIRIT_INSTANTIATE(expression_type, iterator_type, context_type);
        BOOST_SPIRIT_INSTANTIATE(compound_expression_type, iterator_type, context_type);
    }
}