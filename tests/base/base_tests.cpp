#include <catch_main.hpp>

#include "base/base.hpp"

TEST_CASE("Example core test", "[Core]")
{
    auto iface = bgcode::base::create_abstract_interface();
    REQUIRE(iface->foo() == 2);
}
