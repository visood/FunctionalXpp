#include <iostream>
#include "fxpp/list.hpp"
#include "maybe.h"
#include "catch.hpp"

using namespace fxpp::List;

TEST_CASE(
	"cons cell",
	"[ConsCell] [Containers]"
){
  using C= ConsCell<int>;
  const auto emptyCell= C() ;
    
	const auto emptyCellPtr=
    std::make_shared<C>(emptyCell);
      //ConsCell<int>());
	//*emptyCellPtr= ConsCell<int>();
	REQUIRE(
    emptyCellPtr->empty());

  const auto one_cell= C(2);
  CHECK(
    not one_cell.empty());
  CHECK(
    one_cell.get() == 2);
  INFO(
    "one cell CHECKED");

  const auto c1Cell= C(1, emptyCell);
  REQUIRE(
    not c1Cell.empty());
  CHECK(
    c1Cell.get() == 1);
  INFO(
    "c1 cell checked");
	const auto c1Ptr= 
    std::make_shared<C>(C(1, emptyCellPtr));
    //std::make_shared<C>(c1Cell);
	//*c1Ptr = ConsCell<int>(1, emptyCellPtr);
  REQUIRE(
    not c1Ptr->empty());
	CHECK(
    c1Ptr->elem() == 1);
	CHECK(
    c1Ptr->next().empty());
  INFO(
    "c1 ptr checked");
  const auto c2Cell= C(2, c1Ptr);
  REQUIRE(
    not c2Cell.empty());
  INFO(
    "c2 cell is not empty");
  CHECK(
    c2Cell.get() == 2);
  INFO(
    "c2 contains a value of 2");
  REQUIRE(
    not c2Cell.next().empty());
  CHECK(
    c2Cell.next().get() == 1);
  INFO(
    "c2 cell checked");
	const auto c2Ptr=
    std::make_shared<C>(
      C(2, c1Ptr));
  INFO(
    "c2 ptr created");
	*c2Ptr = ConsCell<int>(2, c1Cell);
  INFO(
    "c2 ptr loaded");
  REQUIRE(
    not (c2Ptr->empty()));
  INFO(
    "c2Ptr is not null");
	CHECK(
    c2Ptr->elem() == 2);
  INFO(
    "c2Ptr element is 2");
  REQUIRE(
    c2Ptr->nextPtr());
  REQUIRE(
    not c2Ptr->next().empty());
	CHECK(
    c2Ptr->next().elem() == 1);
  INFO(
    "c2Ptr net elem is 1");
	CHECK(
    c2Ptr->next().next().empty());
  INFO(
    "c2Ptr next next is empty");
}

TEST_CASE (
	"create persistent list",
	"[PersistentLists] [Containers]"
) {
	SECTION("empty lists") {
		CHECK(Nil<int>.empty());
		CHECK(Nil<double>.empty());
		CHECK(Nil<std::string>.empty());
		const auto emptyList = List<int>();
		CHECK(emptyList.empty());
		CHECK(emptyList.size() == 0);
	}

	SECTION("list head tail and size") {
		const auto l0 = List<int>();
		CHECK(l0.size() == 0);
		const auto l1 = cons(1, l0);
		std::cout << "l1 first: ";
		l1.printCellUsage();
		CHECK(l1.size() == 1);
		CHECK(l1.head() == 1);
		CHECK(l1.tail().empty());
		std::cout << "l1 second: ";
		l1.printCellUsage();
		const auto l2 = cons(2, l1);
		std::cout << "l2 first: ";
		l2.printCellUsage();
		std::cout << "l1 third: ";
		l1.printCellUsage();
		CHECK(l2.size() == 2);
		CHECK(l2.head() == 2);
		CHECK(l2.tail().head() == 1);
		CHECK(l2.tail().tail().empty());

		std::cout << "l2 second: ";
		l2.printCellUsage();

		const auto l3 = cons(3, l1);
		CHECK(l3.size() == 2);
		CHECK(l3.head() == 3);
		CHECK(l3.tail().head() == 1);
		CHECK(l3.tail().tail().empty());

		std::cout << "l2 third: ";
		l2.printCellUsage();

		l3.printCellUsage();
	}
	SECTION("operator >> to cons list"){
		const auto l0 = List<int>();
		CHECK(l0.size() == 0);
		const auto l1 = 1 >> l0;
		std::cout << "l1 first: ";
		l1.printCellUsage();
		CHECK(l1.size() == 1);
		CHECK(l1.head() == 1);
		CHECK(l1.tail().empty());
		std::cout << "l1 second: ";
		l1.printCellUsage();
		const auto l2 = 2 >> l1;
		std::cout << "l2 first: ";
		l2.printCellUsage();
		std::cout << "l1 third: ";
		l1.printCellUsage();
		CHECK(l2.size() == 2);
		CHECK(l2.head() == 2);
		CHECK(l2.tail().head() == 1);
		CHECK(l2.tail().tail().empty());

		std::cout << "l2 second: ";
		l2.printCellUsage();

		const auto l3 = 3 >> l1;
		CHECK(l3.size() == 2);
		CHECK(l3.head() == 3);
		CHECK(l3.tail().head() == 1);
		CHECK(l3.tail().tail().empty());

		std::cout << "l2 third: ";
		l2.printCellUsage();

		l3.printCellUsage();
	}
}

TEST_CASE(
	"shared data in persistent lists",
	"[PersistentSharedData][PersistentLists][Containers]"
) {
	SECTION("cons") {
		const auto l0 = List<int>();
		std::cout << "l0 created: ";
		l0.printCellUsage();

		const auto l1 = cons(1, l0);
		std::cout << "l1 created: ";
		l1.printCellUsage();
		std::cout << "from the point of view of l0: ";
		l0.printCellUsage();

		const auto l2 = cons(2, l1);
		std::cout << "l2 created: ";
		l2.printCellUsage();
		std::cout << "from the point of view of l1: ";
		l1.printCellUsage();


		const auto l3 = cons(3, l1);
		std::cout << "l3 created: ";
		l3.printCellUsage();
		std::cout << "from the point of view of l1: ";
		l1.printCellUsage();

		std::cout << "before l4, l3 tail";
		l3.tail().printCellUsage();
		const auto l4 = cons(4, l1);
		std::cout << "l4 created: ";
		l4.printCellUsage();
		std::cout << "after l4, l3 tail";
		l3.tail().printCellUsage();
		std::cout << "from the point of view of l2: ";
		l2.printCellUsage();
		std::cout << "from the point of view of l1: ";
		l1.printCellUsage();

		std::cout << "l4 tail usage";
		l4.tail().printCellUsage();

		std::cout << "l4 tail usage";
		l4.tail().printCellUsage();

		std::cout << "l4 tail usage";
		l4.tail().printCellUsage();
	}
	SECTION(">>") {
		const auto l0 = List<int>();
		std::cout << "l0 created: ";
		l0.printCellUsage();

		const auto l1 = 1 >> l0;
		std::cout << "l1 created: ";
		l1.printCellUsage();
		std::cout << "from the point of view of l0: ";
		l0.printCellUsage();
		CHECK(true);

		const auto l2 = 2 >> l1;
		std::cout << "l2 created: ";
		l2.printCellUsage();
		std::cout << "from the point of view of l1: ";
		l1.printCellUsage();


		const auto l3 = 3 >> l1;
		std::cout << "l3 created: ";
		l3.printCellUsage();
		std::cout << "from the point of view of l1: ";
		l1.printCellUsage();

		std::cout << "before l4, l3 tail";
		l3.tail().printCellUsage();
		const auto l4 = 4 >> l1;
		std::cout << "l4 created: ";
		l4.printCellUsage();
		std::cout << "after l4, l3 tail";
		l3.tail().printCellUsage();
		std::cout << "from the point of view of l2: ";
		l2.printCellUsage();
		std::cout << "from the point of view of l1: ";
		l1.printCellUsage();

		std::cout << "l4 tail usage";
		l4.tail().printCellUsage();

		std::cout << "l4 tail usage";
		l4.tail().printCellUsage();

		std::cout << "l4 tail usage";
		l4.tail().printCellUsage();
	}
}

TEST_CASE(
	"make a persistent list with",
	"[PersistentListMaker][PersistentLists][Containers]"
) {
	const auto l0 = make_list(1, 2, 3, 4, 5);
	CHECK(l0.size() == 5);
	CHECK(head(l0) == 1);
	CHECK(head(tail(l0)) == 2);
	CHECK(head(tail(tail(l0))) == 3);
	CHECK(head(tail(tail(tail(l0)))) == 4);
	CHECK(head(tail(tail(tail(tail(l0))))) == 5);
	CHECK(tail(tail(tail(tail(tail(l0))))).nil());
}

