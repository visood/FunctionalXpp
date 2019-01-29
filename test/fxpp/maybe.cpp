/*
  Test Maybe

  Vishal Sood
  20190128
*/

#include <iostream>
#include "fxpp/maybe.hpp"
#include "catch.hpp"

using namespace fxpp::Maybe;

TEST_CASE(
  "Maybe works as expected",
  "[Maybe][Containers]"
){
  CHECK(
    None<int>().empty);

  const auto& mb0=
    None<int>();
  CHECK(
    mb0.empty);
  const auto mb1=
    Some(1);
  CHECK(
    not mb1.empty);
  CHECK(
    mb1.get() == 1);

  CHECK(
    mb1.map(
      [] (const int x){return 2 * x;}
    ).get()
    ==  2);
  CHECK(
    mb1.filter(
      [] (const int ) {return false;}
    ).empty);
  CHECK(
    mb1.filter(
      [] (const int) {return true;}
    ).get()
    == 1);
}
