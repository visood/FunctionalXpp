/*
  Test the enum macro
*/
#include <iostream>
#include "catch.hpp"

#include "fxpp/enumacro.hpp"
#define COLORENTRIES(COLOR, X)                        \
X(COLOR, red, "Red")                                  \
X(COLOR, green, "Green")                              \
X(COLOR, blue, "Blue")                       

ENUM(Color, COLORENTRIES);
#undef COLORENTRIES
#include "fxpp/enumacro_undef.hpp"


TEST_CASE(
  "enum macro",
  "[ENUM][MACRO]")
{
  CHECK(
    description(Color::red) == "Red");
  CHECK(
    description(Color::blue) == "Blue");
  CHECK(
    description(Color::green) == "Green");
}
