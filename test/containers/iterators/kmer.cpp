#include <numeric>
#include <iostream>
#include <array>
#include "util.h"
#include "kmer.h"
#include "catch.hpp"


TEST_CASE ("Kmer iterator can be used to iterate over the kmers in a vector", "[KmerClassIterator]") {
  std::vector<int> xs(6);
  std::iota(xs.begin(), xs.end(), 0);
  Kmer< std::vector<int>, int, 3 > mer3(xs);
  REQUIRE( mer3.size() == 4);
  int n = 0;
  SECTION("auto works") {
    for( auto kmer: mer3) {
      std::cout << "n: " << n << ", kmer: "
                << kmer[0] << ", " << kmer[1] << ", " << kmer[2] << std::endl;
      REQUIRE( kmer.size() == 3);
      REQUIRE( kmer[0] == n );
      REQUIRE( kmer[1] == n + 1);
      REQUIRE( kmer[2] == n + 2);
      n += 1;
    }
    REQUIRE( n == 4);
  }

  SECTION("accumulate") {
    uint ssize = std::accumulate(mer3.begin(), mer3.end(), (uint) 0,
                                 [] (const uint& s, const auto& kmer) {
                                   return s + kmer.size();
                                 });
    REQUIRE(ssize == 12);
  }
  SECTION( "while done") {
    Kmer<std::vector<int>, int, 3>::iterator kitr(std::begin(xs), std::end(xs));
    kitr.start();
    n = 0;
    while (!kitr.done()) {
      const auto& kmer = *kitr;
      REQUIRE( kmer[0] == n);
      REQUIRE( kmer[1] == n + 1);
      REQUIRE( kmer[2] == n + 2);
      ++kitr; ++n;
    }
  }

  SECTION( "for_each ") {
    Kmer<std::vector<int>, int, 3>::iterator kitr(std::begin(xs), std::end(xs));
    kitr.start();
    n = 0;
    for_each(kitr, [&n] (const auto& kmer) {
        REQUIRE( kmer[0] == n);
        REQUIRE( kmer[1] == n + 1);
        REQUIRE( kmer[2] == n + 2);
        ++n;
      } );
  }
}




/*
 * a tower of inheritence, coded in utils.h, inspired by a class
 * that overloads lambdas!!!
 * move it where it should belong
 */
TEST_CASE("A tower of inheritence", "[inhertower]") {
    std::cout << "carry on" << std::endl;
    auto tow = make_tower(1,-1.0, std::string("one"));
    tow(2);
    tow(-2.0);
    tow(std::string("two"));
}

