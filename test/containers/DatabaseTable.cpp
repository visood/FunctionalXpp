#include <iostream>
#include <string>
#include <map>
#include <tuple>
#include <vector>
#include "util.h"
#include "datatypes.h"
#include "RelationalDatabaseSim.h"
#include "DatabaseTable.h"
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

