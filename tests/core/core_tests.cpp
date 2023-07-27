#include <catch_main.hpp>

#include "core/core.hpp"

TEST_CASE("Example core test", "[Core]")
{
    REQUIRE(bgcode::core::foo() == 1);
}
