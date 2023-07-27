#include "convert.hpp"
#include "base/base.hpp"

namespace bgcode { namespace convert {

BGCODE_CONVERT_EXPORT int foo()
{
    return bgcode::base::foo() + 1;
}

} // namespace core
} // namespace bgcode
