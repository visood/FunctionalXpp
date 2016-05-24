#include <iostream>
#include <string>
#include <map>
#include <tuple>
#include <vector>
#include "util.h"
#include "datatypes.h"
#include "RelationalDatabaseSim.h"
#include "DatabaseTable.h"
#include "DataFrame.h"
#include "TupleFrame.h"
#include "catch.hpp"

TEST_CASE("Database Table typed using a parameter pack", "[DatabaseTable]") {
    using string = std::string;
    std::vector<std::vector<string> > table;
    for (uint i = 0; i != 100; ++i) {
        std::vector<string> row{
            DataType::convert<string, double>((double) i),
            DataType::convert<string, uint>(i),
            wordyInteger(i) };
        table.push_back(row);
    }
    SECTION ("read a result") {
      StrRowRdbTable* res = new StrRowRdbTable(3, table);
      std::cout << "obtained a pointer to a db table sim" << std::endl;
      REQUIRE( res );
      REQUIRE( res->size() == (uint) table.size());
      uint i = 0;
      DatabaseTable< double, int, std::string > dbt("test");
      while(res->next()){
        auto tup = dbt.read(res);
        REQUIRE( std::get<0>(tup) == (double) i);
        REQUIRE( std::get<1>(tup) == (uint) i);
        REQUIRE( std::get<2>(tup) == wordyInteger(i));
        i += 1;
      }
      delete res;
    }

    std::cout << "past read a result" << std::endl;

    SECTION ("load from a result") {
      StrRowRdbTable* res = new StrRowRdbTable(3, table);
      std::cout << "obtained a pointer to a db table sim" << std::endl;
      REQUIRE( res );
      REQUIRE( res->size() == (uint) table.size());
      uint i = 0;
      DatabaseTable< double, int, std::string > dbt("test");
      dbt.loadResult(res);
      REQUIRE(dbt.nrow() == table.size());
      i = 0;
      for (const auto& tup: dbt.data()) {
        REQUIRE( std::get<0>(tup) == (double) i);
        REQUIRE( std::get<1>(tup) == (int) i);
        REQUIRE( std::get<2>(tup) == wordyInteger(i));
        i += 1;
      }
      delete res;
    }

    std::cout << "past load from a result" << std::endl;

    SECTION ("load from a query") {
      StrRowRdbTable strTable = StrRowRdbTable(3, table);
      DbSim dbsim;
      dbsim.insert("table", strTable);
      DbconnClassSim conn(dbsim);
      DbQuerySim& query = conn.query();
      std::cout << "query obtained " << std::endl;
      query << "table";
      std::cout << query.table();
      //StrRowRdbTable const* res = query->execute();
      uint i = 0;
      DatabaseTable< double, int, std::string > dbt("test");
      dbt.loadQuery(query);
      REQUIRE(dbt.nrow() == table.size());
      i = 0;
      for (const auto& tup: dbt.data()) {
        REQUIRE( std::get<0>(tup) == (double) i);
        REQUIRE( std::get<1>(tup) == (int) i);
        REQUIRE( std::get<2>(tup) == wordyInteger(i));        i += 1;
      }      //delete query;
    }
}

TEST_CASE("A tuple of vectors ", "[VectorizedTuple]") {

    std::vector<int> xs(10);
    std::iota(xs.begin(), xs.end(), 0);
    std::vector<std::string> ys(10);
    std::transform(begin(xs), end(xs), begin(ys),
                   [] (const int i) {
                     return wordyInteger(i);
                   });
    //vectorizedTuple<int, std::string>::type xy;
    //ParameterModifiedContainer< std::tuple, std::vector, int, std::string>::type xy;
    VectorizedTuple<int, std::string>::type xy;
    std::get<0>(xy) = xs;
    std::get<1>(xy) = ys;

    int i = 0;
    for(const auto x: std::get<0>(xy))
      REQUIRE (x == i++);
    i = 0;
    for(const auto x: std::get<1>(xy))
      REQUIRE (x == wordyInteger(i++));
}


TEST_CASE("DataFrame that contains a tuple of vectors", "[DataFrame]") {
    using string = std::string;
    std::vector<std::vector<string> > table;
    for (uint i = 0; i != 100; ++i) {
        std::vector<string> row{
            DataType::convert<string, double>((double) i),
            DataType::convert<string, uint>(i),
            wordyInteger(i) };
        table.push_back(row);
    }
    SECTION ("read a result") {
      StrRowRdbTable* res = new StrRowRdbTable(3, table);
      std::cout << "obtained a pointer to a db table sim" << std::endl;
      REQUIRE( res );
      REQUIRE( res->size() == (uint) table.size());
      uint i = 0;
      DataFrame< double, int, std::string > df(res);
      while(res->next()){
        auto tup = df.readRow(res);
        REQUIRE( std::get<0>(tup) == (double) i);
        REQUIRE( std::get<1>(tup) == (uint) i);
        REQUIRE( std::get<2>(tup) == wordyInteger(i));
        i += 1;
      }
      delete res;
    }

    std::cout << "past read a result" << std::endl;

    SECTION ("load from a result") {
      StrRowRdbTable* res = new StrRowRdbTable(3, table);
      std::cout << "obtained a pointer to a db table sim" << std::endl;
      REQUIRE( res );
      REQUIRE( res->size() == (uint) table.size());
      DataFrame< double, int, std::string > df(res);
      REQUIRE(df.nrow() == table.size());
      std::vector<uint> xs(df.nrow());
      std::iota(xs.begin(), xs.end(), 0);
      for (uint x: xs) {
        REQUIRE(df.element<0>(x) == (double) x);
        REQUIRE(df.element<1>(x) == (int) x);
        REQUIRE(df.element<2>(x) == wordyInteger(x));
      }
      delete res;
    }

    SECTION ("named columns in a data-frame") {
      class EgDF : public DataFrame<double, int, std::string> {
      public:
        EgDF(StrRowRdbTable* res) : DataFrame<double, int, std::string>(res) {}

        double vdouble(uint i) { return element<0>(i);}
        int vint(uint i) { return element<1>(i);}
        std::string vstr(uint i) {return element<2>(i);}
      };

      StrRowRdbTable* res = new StrRowRdbTable(3, table);
      EgDF df(res);
      REQUIRE(df.nrow() == table.size());
      std::vector<uint> xs(df.nrow());
      std::iota(xs.begin(), xs.end(), 0);
      for (uint x: xs) {
        REQUIRE(df.element<0>(x) == (double) x);
        REQUIRE(df.element<1>(x) == (int) x);
        REQUIRE(df.element<2>(x) == wordyInteger(x));
        REQUIRE(df.vdouble(x) == (double) x);
        REQUIRE(df.vint(x) == (int) x);
        REQUIRE(df.vstr(x) == wordyInteger(x));
      }
      delete res;
    }

}

TEST_CASE("A result reader", "[Reader]") {
  using string = std::string;
  std::vector<std::vector<string> > table;
  for (uint i = 0; i != 100; ++i) {
    std::vector<string> row{
      DataType::convert<string, double>((double) i),
        DataType::convert<string, uint>(i),
        wordyInteger(i) };
    table.push_back(row);
  }
  StrRowRdbTable* res = new StrRowRdbTable(3, table);
  Reader<StrRowRdbTable*, double, int, std::string> read;
  uint x = 0;
  while(res->next()) {
    auto tup = read(res);
    REQUIRE(std::get<0>(tup) == (double) x);
    REQUIRE(std::get<1>(tup) == (int) x);
    REQUIRE(std::get<2>(tup) == wordyInteger(x++));
  }

}
