//macro CATCH_CONFIG_MAIN will create a main for CATCH
//define this macro only once
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
//#include "FunctionalXpp.h"
#include "RelationalDatabaseSim.h"


std::string wordyOneToNine(uint x) {
  if (x > 9) throw std::invalid_argument("wordyOneToNine cannot wordify number larger than 9");
  if (x == 0) throw std::invalid_argument("wordyOneToNine cannot wordify 0");
  switch (x) {
  case 9:
    return "nine";
  case 8:
    return "eight";
  case 7:
    return "seven";
  case 6:
    return "six";
  case 5:
    return "five";
  case 4:
    return "four";
  case 3:
    return "three";
  case 2:
    return "two";
  case 1:
    return "one";
  case 0:
    return "";
  }
  return "";
}

std::string wordyInteger(uint x) {
  if (x > 1000) throw std::invalid_argument("wordInteger cannot wordify numbers that are larger than 1000");
  std::string wordy;
  if (x > 100) wordy = wordyOneToNine(x/100) + " hundred";
  if (x > 100 and x % 100 > 0)  wordy += " and ";
  x = x % 100;
  switch (x / 10) {
  case (9):
    wordy += "ninety";
    break;
  case (8):
    wordy += "eighty";
    break;
  case (7):
    wordy += "seventy";
    break;
  case (6):
    wordy += "sixty";
    break;
  case (5):
    wordy += "fifty";
    break;
  case (4):
    wordy += "forty";
    break;
  case (3):
    wordy += "thirty";
    break;
  case (2):
    wordy += "twenty";
    break;
  case (1):
    switch (x % 10) {
    case (9):
      wordy += "nineteen";
      return wordy;
    case (8):
      wordy += "eighteen";
      return wordy;
    case (7):
      wordy += "seventeen";
      return wordy;
    case (6):
      wordy += "sixteen";
      return wordy;
    case (5):
      wordy += "fifteen";
      return wordy;
    case (4):
      wordy += "fourteen";
      return wordy;
    case (3):
      wordy += "thirteen";
      return wordy;
    case (2):
      wordy += "twelve";
      return wordy;
    case (1):
      wordy += "eleven";
      return wordy;
    }
  case (0):
    break;
  }
  if (x % 10 == 0) return wordy;
  if (x > 10 ) wordy += " ";
  wordy += wordyOneToNine( x % 10 );
  return wordy;
}

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

/*
TEST_CASE ("DataTypes are available", "[datatypes]") {
    REQUIRE (DoubleType);
    REQUIRE (StringType);
    REQUIRE (IntegerType);
}
*/
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

TEST_CASE ("Simulate a Relational Database Row", "[RDBsim], [RDBrowSim]") {
  std::vector<std::string> rowVec {"1.0", "1", "one"};
  StrRdbRow* row = new StrRdbRow(rowVec);
  REQUIRE (row->getDouble(1) == 1.0);
  REQUIRE (row->getInt(2) == 1);
  REQUIRE (row->getString(3) == "one");
  delete(row);
}

TEST_CASE("Simulate a Relational Database Table", "[RDBSim], [RDBtableSim]") {
  std::vector<std::vector<std::string> > table;
  for (uint i = 0; i != 100; ++i) {
    std::vector<std::string> row{ DataType::convert<std::string, double>((double) i),
        DataType::convert<std::string, uint>(i),
        wordyInteger(i)};
    table.push_back(row);
  }

  StrRowRdbTable* res = new StrRowRdbTable(3, table);
  REQUIRE( res );
  REQUIRE( res->size() == (uint) table.size());
  uint i = 0;
  while (res->next()) {
    //res->printCurrent();
    REQUIRE( res->getDouble(1) == (double) i);
    REQUIRE( res->getInt(2) == (int) i);
    REQUIRE( res->getString(3) == wordyInteger(i));
    i += 1;
  }

  delete res;
}

TEST_CASE("Iterator to a RelationalDatabaseSim works", "[RDBSim], [RDBtableSim], [RDBtableSimIterator]") {
  std::vector<std::vector<std::string> > table;
  for (uint i = 0; i != 100; ++i) {
    std::vector<std::string> row{ DataType::convert<std::string, double>((double) i),
        DataType::convert<std::string, uint>(i),
        wordyInteger(i)};
    table.push_back(row);
  }

  StrRowRdbTable* res = new StrRowRdbTable(3, table);
  REQUIRE( res );
  REQUIRE( res->size() == (uint) table.size());

  //std::cout << "sum doubles using an iterator" << std::endl;
  IterableRDB<StrRowRdbTable> rdbItr(res);

  double sum = std::accumulate(rdbItr.begin(), rdbItr.end(), 0.0,
                               [] (const double s, const auto row) {
                                 //std::cout << row->position() << std::endl;
                                 return s + row->getDouble(1);
                               });

  //std::cout << "summed doubles " << sum << std::endl;
  REQUIRE( sum == 4950);
}
