#include <catch_main.hpp>

#include "base/base.hpp"

TEST_CASE("Example base test", "[Base]")
{
    REQUIRE(bgcode::base::foo() == bgcode::core::foo() + 1);
}
