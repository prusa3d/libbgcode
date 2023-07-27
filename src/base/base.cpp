#include "base.hpp"

namespace bgcode { namespace base {

BGCODE_BASE_EXPORT int foo()
{
    return core::foo() + 1;
}

}} // namespace bgcode
