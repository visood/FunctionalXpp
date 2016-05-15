#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include "util.h"
#include "datatypes.h"
#include "catch.hpp"

TEST_CASE("numbers can be converted to words", "[Wordify]") {
  std::cout <<  wordyInteger(987) << std::endl;
  REQUIRE( wordyInteger(987) == "nine hundred and eighty seven");
  std::cout << wordyInteger(570) << std::endl;
  REQUIRE( wordyInteger(570) == "five hundred and seventy");
  std::cout << wordyInteger(507) << std::endl;
  REQUIRE( wordyInteger(507) == "five hundred and seven");
  std::cout << wordyInteger(500) << std::endl;
  REQUIRE( wordyInteger(500) == "five hundred");
  std::cout << wordyInteger(90) << std::endl;
  REQUIRE( wordyInteger(90) == "ninety");
  std::cout << wordyInteger(19) << std::endl;
  REQUIRE( wordyInteger(19) == "nineteen");
  std::cout << wordyInteger(9) << std::endl;
  REQUIRE( wordyInteger(9) == "nine");
}

TEST_CASE ("Tuples can be printed", "[printTuples]") {
    auto t1 = std::make_tuple(1, 0.0f, "hello");
    std::cout << "size of the tuple "
              << std::tuple_size<std::tuple<int, float, char*>>::value << std::endl;
    std::cout << std::get<0> (t1) << ", " << std::get<1>(t1) << ", " << std::get<2>(t1) << std::endl;
}

TEST_CASE ("Values of a certain type can be obtained from a string", "[DataTypes]") {
    double xdouble;
    int xint;
    std::string xstr;
    DataType::getValue(xdouble, "1.0");
    DataType::getValue(xint, "1");
    DataType::getValue(xstr, "one");
    REQUIRE( xdouble == 1.0);
    REQUIRE( xint == 1);
    REQUIRE( xstr == "one");

    double xd = DataType::convert<double> ("1.0");
    int xi = DataType::convert<int> ("1");
    std::string xs = DataType::convert<std::string> ("one");
    REQUIRE( xd == 1.0);
    REQUIRE( xi == 1);
    REQUIRE( xs == "one");
}
