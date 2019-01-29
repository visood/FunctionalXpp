/*
  Test the enum macro
*/
#include <iostream>
#include "catch.hpp"

#include "fxpp/enumacro.hpp"
#define EXAMPLE(ENUNAME, X)                    \
X(ENUNAME, red, "Red")                         \
X(ENUNAME, green, "Green")
X(ENUNAME, blue, "Blue")                       \

ENUM(Color, EXAMPLE);
#undef EXAMPLE
#include "fxpp/enumacro_undef.hpp"


TEST_CASE(
  "enum macro",
  ["ENUM"] ["MACRO"])
{
  std::cout << Color.red << std::endl;
}
