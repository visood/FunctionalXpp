#include <algorithm>
#include <numeric>
#include <iostream>
#include <array>
#include "util.h"
#include "view.h"
#include "catch.hpp"

TEST_CASE ("View a std::container", "[SimpleViewContainer]") {
  std::vector<int> xs(6);
  std::iota(xs.begin(), xs.end(), 0);
  int i = 0;
  SimpleView xsview(xs);
  for( const auto& x: xs) {
    REQUIRE(x == i++);
  }
  long btob = std::distance(xsview.begin(), xsview.begin());
  INFO("distance begin to begin of view " << btob);
  REQUIRE(btob == 0);
  long etoe = std::distance(xsview.end(), xsview.end());
  INFO("distance end to end of view " << etoe);
  REQUIRE(etoe == 0);
  std::cout << (uint) std::distance(xs.begin(), xs.end()) << std::endl;
  uint N = (uint) xsview.size();
  INFO("length of the view " << N);
  REQUIRE( N ==  6UL);
}
