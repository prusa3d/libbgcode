#include <catch_main.hpp>

#include "convert/convert.hpp"

TEST_CASE("Example convert test", "[Convert]")
{
    REQUIRE(bgcode::convert::foo() == bgcode::core::foo() + 2);
}
