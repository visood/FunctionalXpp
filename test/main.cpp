//macro CATCH_CONFIG_MAIN will create a main for CATCH
//define this macro only once
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
//#include "FunctionalXpp.h"
//#include "RelationalDatabaseSim.h"
#include "DatabaseTable.h"
#include "kmer.h"


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

TEST_CASE("Database Table typed using a parameter pack", "[DatabaseTable]") {
    using string = std::string;
    std::vector<std::vector<string> > table;
    for (uint i = 0; i != 100; ++i) {
        std::vector<string> row{ DataType::convert<string, double>((double) i),
                DataType::convert<string, uint>(i),
                wordyInteger(i)};
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
        REQUIRE( std::get<1>(tup) == (int) i);
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
        REQUIRE( std::get<2>(tup) == wordyInteger(i));
        i += 1;
      }
      //delete query;
    }
}

TEST_CASE ("Kmer iterator can be used to iterate over the kmers in a vector", "[KmerClassIterator]") {
  std::vector<int> xs(6);
  std::iota(xs.begin(), xs.end(), 0);
  Kmer< std::vector<int> > mer3(3, xs);
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
    Kmer<std::vector<int>>::iterator kitr(std::begin(xs), std::end(xs), 3);
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
    Kmer<std::vector<int>>::iterator kitr(std::begin(xs), std::end(xs), 3);
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


