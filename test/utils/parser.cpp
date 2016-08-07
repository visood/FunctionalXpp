#include "catch.hpp"
#include "parser.h"

using namespace Expression;

TEST_CASE(
	"Functional Parser basics",
	"[ParserBasicSyntax] [FunctionalParserBasics]"
)
{
	const String hw("hello world,  bonjour, tous monde,  and namastey jee.");
	//const auto ri = parse(item, hw);
	const auto ri0 = item(hw);
	REQUIRE( not ri0.empty );
	CHECK( ri0.value == 'h');

#if 0
	const auto twoItems = item >>= [=] (const char c1) {
		return item >>= [=] (const char c2) {
			return yield(std::make_tuple(c1, c2));
		};
	};
	const auto helloItem0 = item >>= std::function< Parser<String>(char) > (
		[=] (const char c) {
			return yield(String("hello") + c);
		}
	);
	const auto helloItem = item >>=  [=] (const char c) {
		return [=] (const String& in) {
			return result( String("hi") + c, in);
		};
	};
#endif

	const auto ry = yield(String("hello"))("world");
	REQUIRE( not ry.empty);
	CHECK( ry.value == "hello");
	CHECK( ry.out == "world");

	const auto hiItem = item >>= [=] (const char c) {
		return yield(String("hi") + c);
	};

	const auto ri = hiItem(hw);
	REQUIRE( not ri.empty);
	//CHECK ( ri.value == "hih");

	const auto hihiItem = item >>= [=] (const char c) {
		return yield(String("hi") + c) >>= [=] (const String& s) {
			return yield(String("hi") + s);
		};
	};
	const auto ri2 = hihiItem(hw);
	REQUIRE( not ri2.empty);
	CHECK(ri2.value == "hihih");

	//not even such an ugly syntax to sequence parsers
	const auto threeItems = (
		item >>= [=] (const char c1) {
			return item >>= [=] (const char c2) {
				return item >>= [=] (const char c3) {
					return yield(String(1, c3) + c2 + c1);
				};
			};
		}
	);

	const auto ri3 = threeItems(hw);
	REQUIRE( not ri3.empty);
	CHECK( ri3.value == "leh");


	//drop some parsed results
	const auto fourthItem = threeItems >> item ;
		
	const auto r4i = parse(fourthItem, hw);
	REQUIRE(not r4i.empty);
	CHECK( r4i.value == 'l');

	//hello the 4th character
	const auto hello4th = threeItems >> item >>= [=] (const char c) {
		return yield (String("hello") + c);
	};

	const auto r4h = parse(hello4th, hw);
	REQUIRE(not r4h.empty);
	CHECK( r4h.value == "hellol");

}

TEST_CASE("repeats", "[FunctionalParserBasics] [Repeat]")
{
	const String hw("hello world,  bonjour, tous monde,  and namastey jee.");
	const auto r3 = parse(repeat<3>(item), hw);
	REQUIRE(not r3.empty);
	CHECK( r3.value == std::make_tuple('h', 'e', 'l'));

	const auto r4 = parse(item & item & item & item, hw);
	REQUIRE(not r4.empty);
	CHECK( r4.value == std::make_tuple('h', 'e', 'l', 'l'));

	const auto rm = parse(many(item), "hoho");
	REQUIRE(not rm.empty);
	const List<char> clist {'h', 'o', 'h', 'o'};
	CHECK(rm.value == clist);
}

TEST_CASE("sat parser", "[FunctionalParserBasics] [Satisfy]")
{
	REQUIRE(parse(char_('x'), "hello").empty);
	const auto rh = parse(char_('h'), "hello");
	REQUIRE(not rh.empty);
	CHECK( rh.value == 'h');

	const auto rs1 = parse(space, "hello");
	REQUIRE(rs1.empty);
	const auto rs2 = parse(space, " hello");
	REQUIRE(not rs2.empty);
	CHECK(rs2.value == ' ');
}
