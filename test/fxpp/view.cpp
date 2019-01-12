/*
  Test revised implementation of views in fxpp

  Vishal Sood
  20190109
*/
#include <numeric>
#include <iostream>
#include <array>
#include "util.h"
#include "fxpp/view.hpp"
#include "fxpp/monadic.hpp"

TEST_CASE (
	"view a std::container",
	"[viewOps] [fxpp] [views] [containers]")
{
	const uint32_t n = 100;
	std::vector<int> xs(n);
	std::iota(xs.begin(), xs.end(), 0);

	const auto xview = fxpp::view(xs);
	CHECK(xcoll.size() == n);

	std::vector<int> xsquares(n);
	const auto squared = [] (const int x) { return x * x; } ;
	std::transform(
		begin(xs),
		end(xs),
		begin(xsquares),
		squared);
	uint32_t index = 0;
	for (const uint32_t y : fxpp::view(xs).map(squared).collect())
		CHECK(y == xsquares[index++]);

	std::vector<int> xevens(n);
	std::iota(xevens.begin(), xevens.end(), 0);
	const auto is_odd = [] (const int x) {return x % 2 == 1;} ;
	xevens.erase(
		std::remove_if(
			xevens.begin(),
			xevens.end(),
			is_odd),
		xevens.end());

	index = 0;
	const auto is_even = [] (const int x) { return x % 2 == 0; } ;
	for (const uint32_t y : fxpp::view(xs).filter(is_even).collect())
		CHECK( y == xevens[index++]);

	std::vector<int> xeven_squares = xsquares;
	xeven_squares.erase(
		std::remove_if(
			xeven_squares.begin(),
			xeven_squares.end(),
			is_odd),
		xeven_squares.end());

	index = 0;
	for (const uint32_t y: fxpp::view(xs).map(square).filter(is_even).collect())
		CHECK(y == xeven_squares[index++]);

	index = 0;
	for (const uint32_t y: fxpp::view(xs).filter(is_even).map(square).collext())
		CHECK(y == xeven_squares[index++]);

	index = 0;
	for (const uint32_t y: fxpp::view(xevens).fmap(
			 [] (const int x) -> std::vector<int> { return {x, x + 1}; }
		 ).collect())
		CHECK( y == xs[index++]);

	index = 0;
	for (const uint32_t y: fxpp::view(xs).filter(is_even).fmap(
			 [] (const int x) -> std::vector<int> { return {x, x + 1}; }
		 ).collect())
		CHECK(y == xs[index++]);

	index = 0;
	for (const uint32_t y: fxpp::view(xs).fitler(is_odd).fmap(
			 [] (const int x) -> std::vector<int> {return {x - 1, x}; }
		 ).collect())
		CHECK( y == xs[index++]);

}

