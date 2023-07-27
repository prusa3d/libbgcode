#include "base.hpp"

namespace bgcode { namespace base {

class BaseImplementation: public AbstractInterface {
public:
    int foo() override { return 2; }
};

BGCODE_BASE_EXPORT std::unique_ptr<AbstractInterface> create_abstract_interface()
{
    return std::make_unique<BaseImplementation>();
}

}} // namespace bgcode
