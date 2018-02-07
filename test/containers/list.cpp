#include <iostream>
#include "list.h"
#include "catch.hpp"

using namespace Persistent::List;

TEST_CASE(
	"cons cell",
	"[ConsCell] [Containers]"
){
	const auto emptyCell = std::make_shared<ConsCell<int>>();
	*emptyCell = ConsCell<int>();

	CHECK(emptyCell->empty());

	const auto c1Ptr = std::make_shared<ConsCell<int>>();
	*c1Ptr = ConsCell<int>(1, emptyCell);
	CHECK(c1Ptr->elem() == 1);
	CHECK(c1Ptr->next().empty());

	std::cout << "c1 checked" << std::endl;
	const auto c2Ptr = std::make_shared<ConsCell<int>>();
	*c2Ptr = ConsCell<int>(2, c1Ptr);
	std::cout << "c2 created" << std::endl;
	CHECK(c2Ptr->elem() == 2);
	CHECK(c2Ptr->next().elem() == 1);
	CHECK(c2Ptr->next().next().empty());
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
}

TEST_CASE(
	"shared data in persistent lists",
	"[PersistentSharedData][PersistentLists][Containers]"
) {
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
