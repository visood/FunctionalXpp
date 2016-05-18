#include <numeric>
#include <iostream>
#include <array>
#include "util.h"
#include "view.h"
#include "catch.hpp"

TEST_CASE ("View a std::container", "[ViewContainer]") {
  std::vector<int> xs(6);
  std::iota(xs.begin(), xs.end(), 0);
  int i = 0;
  for_each(View<std::vector, int, int>::begin(xs),
           View<std::vector, int, int>::end(xs),
           [&i] (const int x) {
             REQUIRE(x == i++);
           });
  REQUIRE(std::distance(View<std::vector, int, int>::begin(xs),
                        View<std::vector, int, int>::end(xs))
          == 6
    );
}
