#include <iostream>
#include "maybe.h"
#include "catch.hpp"



using namespace Persistent;
Maybe::Maybe<int> getSome(const int x)
{
	return Maybe::Maybe<int>(x);
}

Maybe::Maybe<int> getNone()
{
	return Maybe::Maybe<int>();
}

TEST_CASE(
	"Maybe works as expected",
	"[Maybe][Containers]"
) {
	CHECK( Maybe::hasNone( Maybe::None<int>) );


	const auto& mb0 = Maybe::None<int>;
	CHECK( Maybe::hasNone(mb0) );

	const auto& mb01 = getNone();
	CHECK( Maybe::hasNone(mb01) );

	const auto mb = Maybe::Some(1);
	CHECK( Maybe::hasSome(mb) );
	CHECK( mb.get() == 1 );

	const auto mb1 = getSome(1);
	CHECK( Maybe::hasSome(mb1) );
	CHECK( mb1.get() == 1 );
}
