#include <catch_main.hpp>

#include "core/core.hpp"

TEST_CASE("Example core test", "[Core]")
{
    auto iface = bgcode::core::create_abstract_interface();
    REQUIRE(iface->foo() == 1);
}
