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

	const auto rs = parse(several(item), "hoho");
	REQUIRE(not rs.empty);
	CHECK(rs.value == clist);

	const auto rf = parse(freq(item), "hoho" );
	REQUIRE(not rf.empty);
	CHECK(rf.value == 4U);
}

TEST_CASE("sat parser", "[FunctionalParserBasics] [Satisfy]")
{
	REQUIRE(parse(char_('x'), "hello").empty);
	const auto rh = parse(char_('h'), "hello");
	REQUIRE(not rh.empty);
	CHECK( rh.value == 'h');

	const auto rs1 = parse(char_(' '), "hello");
	REQUIRE(rs1.empty);
	const auto rs2 = parse(char_(' '), " hello");
	REQUIRE(not rs2.empty);
	CHECK(rs2.value == ' ');
}

TEST_CASE("spacing", "[FunctionalParserBasics] [Spacing]")
{
	const auto sp0 = token(item);

	const auto rs0 = parse(sp0, "     x     ");
	REQUIRE(not rs0.empty);
	CHECK(rs0.value == 'x');

}

TEST_CASE("regex capture", "[Regex]")
{
	const Parser< uint32_t > uintx = (
		capture("[[:digit:]]+") >>= [=] (const String& si) {
			return yield((uint32_t) std::stoul(si));
		}
	);
	const auto ri1 = parse(uintx, "1212.hello");
	REQUIRE(not ri1.empty);
	CHECK(ri1.value == 1212U);
	const auto ri2 = parse(uintx, "hello.1212");
	REQUIRE(ri2.empty);


	const Parser< int32_t > esintx = (
		(char_('+') | char_('-')) >>= [=] (const char c) {
			return capture("[[:digit:]]+") >>= [=] (const String& xs) {
				return yield((c == '+' ? 1 : -1) * std::stoi(xs));
			};
		}
	);
	const auto sintx = esintx | (uintx  >>= [=](const uint32_t u) {
			return yield((int32_t) u);
		}
	);

	const auto ri3 = parse(sintx, "+1212.hello");
	REQUIRE(not ri3.empty);
	CHECK(ri3.value == 1212);
	const auto ri4 = parse(sintx, "-1212.hello");
	REQUIRE(not ri4.empty);
	CHECK(ri4.value == -1212);
	const auto ri5 = parse(sintx, "1212.hello");
	REQUIRE(not ri5.empty);
	CHECK(ri5.value == 1212);
	const auto ri6 = parse(sintx, "*1212.hello");
	REQUIRE(ri6.empty);

	const auto cdotx0 = char_('c') >> char_('.') >> (
		( char_('*') | char_('-') | yield('+') ) & uintx
	);
	const auto cdotx = char_('c') >> char_('.') >> yield();
	const auto cposx = ( char_('*') | char_('-') | yield('+') ) & uintx;
	const auto rposx = esintx | yield((int32_t) 0);
	const auto tposx = (
		cdotx >>= [=]() {
			return cposx >>= [=](const auto& cp) {
				return rposx >>= [=](const auto& r) {
					return yield(std::make_tuple(cp, r));
				};
			};
		}
	);
	const auto rtp1 = parse(tposx, "c.312-56");
	CHECK(std::get<0>(rtp1.value) == std::make_tuple('+', 312) );
	CHECK(std::get<1>(rtp1.value) == -56);
	const auto rtp2 = parse(tposx, "c.-312+56");
	CHECK(std::get<0>(rtp2.value) == std::make_tuple('-', 312) );
	CHECK(std::get<1>(rtp2.value) == 56);
	const auto rtp3 = parse(tposx, "c.*312");
	CHECK(std::get<0>(rtp3.value) == std::make_tuple('*', 312) );
	CHECK(std::get<1>(rtp3.value) == 0);


#if 0
	>>= [=] (const char c) {
			return uintx >>= [=] (const auto u) {
				return yield(std::make_tuple(c, u) );
			};
		}
	);
		(char_('*') >> uintx >>= [=](const auto u) {
			return yield (std::make_tuple('*', u));
		}) |
		(char_('-') >> uintx >>= [=](const auto u) {
			return yield (std::make_tuple('-', u));
		}) |
		(uintx >>= [=](const auto u) {
			return yield (std::make_tuple('+', u));
		})
	);
	const auto rc1 = parse(cdotx, "c.*1212.jasjasj");
	REQUIRE(not rc1.empty);
	CHECK(rc1.value == std::make_tuple('*', 1212));
	const auto rc2 = parse(cdotx, "c.-1212.jasjasj");
	REQUIRE(not rc2.empty);
	CHECK(rc2.value == std::make_tuple('-', 1212));
	const auto rc3 = parse(cdotx, "c.1212.jasjasj");
	REQUIRE(not rc3.empty);
	CHECK(rc3.value == std::make_tuple('+', 1212));
	const auto rc4 = parse(cdotx, "c.+1212.jasjasj");
	REQUIRE(rc4.empty);
#endif
}

TEST_CASE(
	"no-value parsed result",
	"[NoValueParsedResult] [FunctionalParserBasics]"
)
{
	const auto nvpr = ParsedResult<void>("hello");
	REQUIRE(not nvpr.empty);
	CHECK( nvpr.out == "hello");

	const auto pv = Parser<void> (
		[=] (const String& in) {
			const auto rx = parse(char_('x'), in);
			return (rx.empty ? empty<void> : some(rx.out));
		}
	);

	const auto rv = parse(pv, "xhello");
	REQUIRE(not rv.empty);
	CHECK(rv.out == "hello");
	const auto rv1 = parse(pv, "hello");
	REQUIRE(rv1.empty);

	const auto pvh = pv >>= [=]() {return yield(String("hello"));};
	const auto rvh = parse(pvh, "xworld");
	REQUIRE(not rvh.empty);
	CHECK( rvh.value == "hello");
	CHECK( rvh.out == "world");
	REQUIRE(parse(pvh, "hello").empty);


	const auto pvh2 = pv >>= [=]() {return yield();};
	const auto rvh2 = parse(pvh2, "xworld");
	REQUIRE( not rvh2.empty);
	CHECK( rvh2.out == "world");
	REQUIRE(parse(pvh2, "hello").empty);

	const auto pvh3 = pv >>= [=]() {return pv;};
	const auto rvh3 = parse(pvh3, "xxworld");
	REQUIRE( not rvh3.empty);
	CHECK( rvh3.out == "world");
	REQUIRE(parse(pvh3, "xworld").empty);

	const auto pvh4 = pvh3 >>= [=]() {return pv;};
	const auto rvh4 = parse(pvh4, "xxxworld");
	REQUIRE( not rvh4.empty);
	CHECK( rvh4.out == "world");
	REQUIRE( parse(pvh4, "xxworld").empty);
}

TEST_CASE(
	"Experiment with nicer syntax",
	"[NicerParserSyntax]"
)
{
	SECTION("Convert strings to intergers") {
		//problem converting strings to ints
		//what happens to an empty string
		String msg;
		try{
			const uint32_t u = (uint32_t) std::stoul(msg);
		} catch (const std::invalid_argument& e) {
			msg += ": caught exception";
		}
		INFO("converting to uint " << msg);
		REQUIRE(not msg.empty());
		CHECK(msg.find("caught exception") != std::string::npos);
	}

	const Parser< uint32_t > uintx = (
		capture("[[:digit:]]+") >>= [=] (const String& si) {
			return yield((uint32_t) std::stoul(si));
		}
	);

	const auto rc = parse(capture("[[:digit:]]+"), "hello world");
	INFO("capture digits in hello world " << rc.value << ", " << rc.out);
	REQUIRE(rc.empty);

	const auto r0 = parse(uintx, "hello world");
	std::cout << "parsed uintx in hello world " << r0.value << ", " << r0.out
			  << std::endl;
	INFO("parse uintx in hello world: " << r0.value << ", " << r0.out);
	REQUIRE(r0.empty);

	const auto twoTimes = do_< Parser<uint32_t> >::yield(
		[] (const auto u) {return 2 * u;}
	);

	const auto r = parse(twoTimes(uintx), "1212");
	REQUIRE(not r.empty);
	CHECK(r.value == 2424);

	const auto r1 = parse(twoTimes(uintx), "hello world");
	INFO("parse twotimes hello world: " << r1.value << ", " << r1.out);
	REQUIRE(r1.empty);


}

