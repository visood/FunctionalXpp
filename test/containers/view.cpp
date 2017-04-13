#include <numeric>
#include <iostream>
#include <array>
#include "util.h"
#include "view.h"
#include "catch.hpp"

TEST_CASE (
	"View a std::container",
	"[BaseView] [Views] [Containers]"
){
	const uint32_t n = 100;
	std::vector<int> xs(n);
	std::iota(xs.begin(), xs.end(), 0);
	
	const auto xview = view(xs);
	CHECK(xview.size() == n);
	
	std::vector<int> xsquares(n);
	const auto square = [] (const int x) {return x * x;};
	std::transform(begin(xs), end(xs), begin(xsquares), square);
				   
	uint32_t index = 0;
	for(const uint32_t y : view(xs).map(square).collect())
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
}
