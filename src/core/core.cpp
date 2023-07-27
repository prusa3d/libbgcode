#include "core.hpp"

#include "libbgcode.hpp"

namespace bgcode { namespace core {

class CoreImplementation: public AbstractInterface {
public:
    int foo() override { return 1; }
};

BGCODE_CORE_EXPORT std::unique_ptr<AbstractInterface> create_abstract_interface()
{
    return std::make_unique<CoreImplementation>();
}

} // namespace core
} // namespace bgcode
