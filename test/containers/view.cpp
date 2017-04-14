#include <numeric>
#include <iostream>
#include <array>
#include "util.h"
#include "view.h"
#include "ChainableView.h"
#include "catch.hpp"

TEST_CASE (
	"view a std::container",
	"[viewOps] [views] [containers]"
){
	const uint32_t n = 100;
	std::vector<int> xs(n);
	std::iota(xs.begin(), xs.end(), 0);
	
	const auto xcollection = collectionView(xs);
	CHECK(xcollection.size() == n);
	
	std::vector<int> xsquares(n);
	const auto square = [] (const int x) {return x * x;};
	std::transform(begin(xs), end(xs), begin(xsquares), square);
				   
	uint32_t index = 0;
	for(const uint32_t y : collectionView(xs).map(square).collect())
		CHECK(y == xsquares[index++]);

	std::vector<int> xevens(n);
	std::iota(xevens.begin(), xevens.end(), 0);
	const auto is_odd = [] (const int x) {return x % 2 == 1; };
	xevens.erase(
		std::remove_if(xevens.begin(), xevens.end(), is_odd),
		xevens.end()
	);

	index = 0;
	const auto is_even = [] (const int x) {return x % 2 == 0; };
	for(const uint32_t y : collectionView(xs).filter(is_even).collect())
		CHECK( y == xevens[index++]);

	std::vector<int> xevenSquares = xsquares;
	xevenSquares.erase(
		std::remove_if(xevenSquares.begin(), xevenSquares.end(), is_odd),
		xevenSquares.end()
	);

	index = 0;
	for(const uint32_t y : collectionView(xs).map(square).filter(is_even).collect())
		CHECK( y == xevenSquares[index++]);

	index = 0;
	for(const uint32_t y : collectionView(xs).filter(is_even).map(square).collect())
		CHECK( y == xevenSquares[index++]);

	//flatMap
	index = 0;
	for(const uint32_t y : collectionView(xevens).flatMap(
			[] (const int x) -> std::vector<int> {return {x, x + 1}; }
		).collect())
		CHECK( y == xs[index++]);

	index = 0;
	for(const uint32_t y : collectionView(xs).filter(is_even
		).flatMap(
			[] (const int x) -> std::vector<int> {return {x, x + 1};}
		).collect())  {
		CHECK( y == xs[index++]);
	}

	index = 0;
	for(const uint32_t y : collectionView(xs).filter(is_odd
		).flatMap(
			[] (const int x) -> std::vector<int> {return {x - 1, x};}
		).collect())  {
		CHECK( y == xs[index++]);
	}

}

TEST_CASE(
	"chainable view",
	"[chainableViews] [views] [containers]"
)
{
	using namespace collection;
	
	const uint32_t n = 10;
	std::vector<int> xs(n);
	std::iota(xs.begin(), xs.end(), 0);
	
	const auto xview = view(xs);

	CHECK(xview.size() == n);

	CHECK(xview.hasBeenCollected());

	std::vector<int> xsquares(n);
	const auto square = [] (const int x) {return x * x;};
	std::transform(begin(xs), end(xs), begin(xsquares), square);
				   
	std::cout << "chained collected from view map" << std::endl;
	uint32_t index = 0;
	for(const uint32_t y : view(xs).map(square).collect())
		CHECK(y == xsquares[index++]);
	std::cout << "=========================================" << std::endl;

	std::cout << "view map, collect later" << std::endl;
	const auto xsquaresView = view(xs).map(square);
	CHECK(not xsquaresView.hasBeenCollected());
	index = 0;
	for(const uint32_t y : xsquaresView.collect())
		CHECK(y == xsquares[index++]);
	std::cout << "=========================================" << std::endl;

	std::cout << "view map, already collected" << std::endl;
	CHECK(xsquaresView.hasBeenCollected());
	index = 0;
	for(const uint32_t y : xsquaresView.collect())
		CHECK(y == xsquares[index++]);
	std::cout << "=========================================" << std::endl;
	
	
	std::cout << "test filter and flat map" << std::endl;
	std::vector<int> xevens(n);
	std::iota(xevens.begin(), xevens.end(), 0);
	const auto is_odd = [] (const int x) {return x % 2 == 1; };
	xevens.erase(
		std::remove_if(xevens.begin(), xevens.end(), is_odd),
		xevens.end()
	);

	index = 0;
	const auto is_even = [] (const int x) {return x % 2 == 0; };
	for(const uint32_t y : view(xs).filter(is_even).collect())
		CHECK( y == xevens[index++]);

	std::vector<int> xevenSquares = xsquares;
	xevenSquares.erase(
		std::remove_if(xevenSquares.begin(), xevenSquares.end(), is_odd),
		xevenSquares.end()
	);

	index = 0;
	for(const uint32_t y : view(xs).map(square).filter(is_even).collect())
		CHECK( y == xevenSquares[index++]);

	index = 0;
	for(const uint32_t y : view(xs).filter(is_even).map(square).collect())
		CHECK( y == xevenSquares[index++]);

	//flatMap
	index = 0;
	for(const uint32_t y : view(xevens).flatMap(
			[] (const int x) -> std::vector<int> {return {x, x + 1}; }
		).collect())
		CHECK( y == xs[index++]);

	index = 0;
	for(const uint32_t y : view(xs).filter(is_even
		).flatMap(
			[] (const int x) -> std::vector<int> {return {x, x + 1};}
		).collect())  {
		CHECK( y == xs[index++]);
	}

	index = 0;
	for(const uint32_t y : view(xs).filter(is_odd
		).flatMap(
			[] (const int x) -> std::vector<int> {return {x - 1, x};}
		).collect())  {
		CHECK( y == xs[index++]);
	}

}
