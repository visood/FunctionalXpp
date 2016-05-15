#include <vector>
#include <map>
#include <tuple>
#include <array>
#include "util.h"
#include "datatypes.h"
#include "RelationalDatabaseSim.h"
#include "catch.hpp"


TEST_CASE("Dispatch on a tuple type", "[TupleDispatch]") {
    using string = std::string;
    std::vector<std::vector<string> > table;
    for (uint i = 0; i != 100; ++i) {
        std::vector<string> row{ DataType::convert<string, double>((double) i),
                DataType::convert<string, uint>(i),
                wordyInteger(i)};
        table.push_back(row);
    }

    //StrRowRdbTable* res = new StrRowRdbTable(3, table);
    //NamedStrRowRdbTable<string, string, string>* res = new NamedStrRowRdbTable<string, string, string>("first", "second", "third");

    StrRowRdbTable* res = new StrRowRdbTable(3, table);
    std::cout << "obtained a pointer to a db table sim" << std::endl;
    REQUIRE( res );


    REQUIRE( res->size() == (uint) table.size());
    int i = 0;
    while (res->next()) {
      REQUIRE( (ArgPack<double, int, std::string>::Element(res, int_<0>()) == (double) i));
      REQUIRE( (ArgPack<double, int, std::string>::Element(res, int_<1>()) == (int) i));
      REQUIRE( (ArgPack<double, int, std::string>::Element(res, int_<2>()) == wordyInteger(i)));
      i += 1;
    }

    delete res;
}

TEST_CASE("Extract values from ResType, typed by a parameter pack", "[ParameterPackTypedValues]") {
    using string = std::string;
    std::vector<std::vector<string> > table;
    for (uint i = 0; i != 100; ++i) {
        std::vector<string> row{ DataType::convert<string, double>((double) i),
                DataType::convert<string, uint>(i),
                wordyInteger(i)};
        table.push_back(row);
    }

    //StrRowRdbTable* res = new StrRowRdbTable(3, table);
    //NamedStrRowRdbTable<string, string, string>* res = new NamedStrRowRdbTable<string, string, string>("first", "second", "third");

    StrRowRdbTable* res = new StrRowRdbTable(3, table);
    std::cout << "obtained a pointer to a db table sim" << std::endl;
    REQUIRE( res );


    REQUIRE( res->size() == (uint) table.size());
    uint i = 0;

    while(res->next()){
      auto tup = getTupleValue<StrRowRdbTable*, double, int, std::string>(res, 0);
      //std::cout << std::get<0>(tup) << ", " << std::get<1>(tup) << ", " << std::get<2>(tup) << std::endl;
      REQUIRE( std::get<0>(tup) == (double) i);
      REQUIRE( std::get<1>(tup) == (int) i);
      REQUIRE( std::get<2>(tup) == wordyInteger(i));
      i += 1;
    }


    delete res;
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

TEST_CASE("Header containing a tuple of column names", "[tableHeader]") {
    //auto hdr = Header<string, string, string>("first", "second", "third");
    auto hdr = header("first", "second", "third");
    REQUIRE( hdr.str() == "first\tsecond\tthird") ;
    //auto hdr2 = Header<string, string, string, string>("first", "second", "third", "fourth");
    auto hdr2 = header("first", "second", "third", "fourth");
    REQUIRE( hdr2.str() == "first\tsecond\tthird\tfourth");
}

TEST_CASE("An index map for names in a header", "[headerIndexMap]") {
    auto idxmap = tupindexes("first", "second", "third");
    REQUIRE ( (idxmap.size() == 3 and
               (idxmap["first"] == 0) and
               (idxmap["second"] == 1) and
               (idxmap["third"] = 2) )
        );
}

TEST_CASE("Simulate a Relational Database Table with column names", "[RDBSim], [RDBtableSim], [RDBtableWithHeader]") {
    using string = std::string;
    std::vector<std::vector<string> > table;
    for (uint i = 0; i != 100; ++i) {
        std::vector<string> row{ DataType::convert<string, double>((double) i),
                DataType::convert<string, uint>(i),
                wordyInteger(i)};
        table.push_back(row);
    }

    //StrRowRdbTable* res = new StrRowRdbTable(3, table);
    //NamedStrRowRdbTable<string, string, string>* res = new NamedStrRowRdbTable<string, string, string>("first", "second", "third");

    const auto res = ptrNamedStrRowRdbTable("first", "second", "third");
    std::cout << "obtained a pointer to a named table" << std::endl;
    REQUIRE( res );


    res->load(table);
    REQUIRE( res->size() == (uint) table.size());
    uint i = 0;
    while (res->next()) {
        REQUIRE( res->getDouble("first") == (double) i);
        REQUIRE( res->getInt("second") == (int) i);
        REQUIRE( res->getString("third") == wordyInteger(i));
        i += 1;
    }

    delete res;
}

