//macro CATCH_CONFIG_MAIN will create a main for CATCH
//define this macro only once
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
//#include "FunctionalXpp.h"
#include "DatabaseTable.h"


TEST_CASE ("Tuples can be printed", "[printTuples]") {
    auto t1 = std::make_tuple(1, 0.0f, "hello");
    std::cout << "size of the tuple "
              << std::tuple_size<std::tuple<int, float, char*>>::value << std::endl;
    std::cout << std::get<0> (t1) << ", " << std::get<1>(t1) << ", " << std::get<2>(t1) << std::endl;
    std::cout << t1 << std::endl;
}

/*
TEST_CASE ("DataTypes are available", "[datatypes]") {
    REQUIRE (DoubleType);
    REQUIRE (StringType);
    REQUIRE (IntegerType);
}
*/
TEST_CASE ("Values of a certain type can be obtained from a string", "[getValue]") {
    double xdouble;
    int xint;
    std::string xstr;
    getValue(xdouble, "1.0");
    getValue(xint, "1");
    getValue(xstr, "one");
    REQUIRE( xdouble == 1.0);
    REQUIRE( xint == 1);
    REQUIRE( xstr == "one");

    double xd = DataType::get<double> ("1.0");
    int xi = DataType::get<int> ("1");
    std::string xs = DataType::get<std::string> ("one");
    REQUIRE( xd == 1.0);
    REQUIRE( xi == 1);
    REQUIRE( xs == "one");
}




