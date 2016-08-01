#include "catch.hpp"
#include "parser.h"

using namespace Expression;

TEST_CASE("Functional Parser basics", "[FunctionalParserBasics]")
{
	const String hw("hello world  bonjour tous monde  and namastey jee.");

	SECTION("item -- a parser that extracts characters from a string ") {
		//an example do while loop to get characters from an input string
		INFO("test parser item");
		const String str0("hello world");
		uint i = 0;
		ParsedResult<char> r = parse(item, str0);
		do {
			CHECK(r.value == str0[i++]);
			r = parse(item, r.out);
		} while (not r.empty);

	}

	SECTION("sat -- parser that checks if an item (character) satisfies  a predicate") {

		const Parser<char> ish = sat(
			[] (const char c) {return c == 'h';}
		);

		INFO("test parser sat");
		const auto rh = parse(ish, hw);
		REQUIRE(not rh.empty);
		CHECK(rh.value == 'h');
	}
	SECTION("word -- parsers to extract a word from a string ") {
		const auto rw = parse(word, hw);
		REQUIRE(not rw.empty);
		CHECK(rw.value == "hello");
		//std::cout << "a word: " << rw.value << ", " << rw.out << std::endl;
	}


	SECTION("sequence parsers -- define compound parsers as sequences of words") {
		const auto oneWord = repeat(word, int_<1>());
		const auto r1w = parse(oneWord, hw);
		INFO("test sequenced parses, one");
		REQUIRE(not r1w.empty);
		CHECK(std::get<0>(r1w.value) == "hello");

		INFO("test sequenced parses, two");
		const auto twoWords = repeat(word, int_<2>());
		const auto r2w = parse(twoWords, hw);
		REQUIRE(not r2w.empty);
		CHECK(std::get<0>(r2w.value) == "hello");
		CHECK(std::get<1>(r2w.value) == "world");

		INFO("test sequenced parses, three");
		const auto threeWords = repeat(word, int_<3>());
		const auto r3w = parse(threeWords, hw);
		REQUIRE(not r3w.empty);
		CHECK(std::get<0>(r3w.value) == "hello");
		CHECK(std::get<1>(r3w.value) == "world");
		CHECK(std::get<2>(r3w.value) == "bonjour");

		INFO("test sequenced parses, six");
		const auto sixWords = repeat(word, int_<6>());
		const auto r6w = parse(sixWords, hw);
		REQUIRE(not r6w.empty);
		CHECK(std::get<0>(r6w.value) == "hello");
		CHECK(std::get<1>(r6w.value) == "world");
		CHECK(std::get<2>(r6w.value) == "bonjour");
		CHECK(std::get<3>(r6w.value) == "tous");
		CHECK(std::get<4>(r6w.value) == "monde");
		CHECK(std::get<5>(r6w.value) == "and");

		const auto rall = parse(split('\n'), hw);
		//std::cout << "scraped all " << rall.value << std::endl;
		INFO("test sequenced parses, seven");
		const auto sevenWords = repeat(word, int_<7>());
		const auto r7w = parse(sevenWords, hw);
		REQUIRE(not r7w.empty);
		CHECK(std::get<0>(r7w.value) == "hello");
		CHECK(std::get<1>(r7w.value) == "world");
		CHECK(std::get<2>(r7w.value) == "bonjour");
		CHECK(std::get<3>(r7w.value) == "tous");
		CHECK(std::get<4>(r7w.value) == "monde");
		CHECK(std::get<5>(r7w.value) == "and");
		CHECK(std::get<6>(r7w.value) == "namastey");


	}
	#if 0
	const auto pq2w = parseq(word).followed_by(word);
	const auto rq2w = parse(pq2w, hw);
	std::cout << "two words parsed with a Parseq: "
						<< std::get<0>(rq2w.value) << ", "
						<< std::get<1>(rq2w.value) << std::endl;
	const auto pte = word | yield(String("Nothing"));
	const auto rte = parse(pte, hw);
	std::cout << "parser or else: "
						<< rte.value << ", " << rte.out << std::endl;
	const auto rte1 = parse(pte, String(""));
	std::cout << "parser or else an empty string: "
						<< rte1.value << ", " << rte1.out << std::endl;
	//list head tail
	List<int> xs {1};
	const auto ys = 0 >= (1 >= (2 >= nil<int>()));
	std::cout << " head listed " << head(ys) << std::endl;
	std::cout << " head of tail " << head(tail(ys)) << std::endl;
	std::cout << "head of tail of tail "
						<< head(tail(tail(ys))) << std::endl;


	const auto allwords = many(word);
	const auto raw = parse(allwords, hw);
	std::cout << "how many words ? " << raw.value.size() << std::endl;
	for (const auto w : raw.value)
		std::cout << w << std::endl;
#endif

	#if 0
	const auto pe = item >= orElse(yield('!'));
	const auto re = parse(pe, String());
	std::cout << "parse with fall back: "
						<< re.value << std::endl;

	const auto re = parse(
		startWith(item)
		.followed_by(word)
		.orElse(yield('!'))
		, String()
	);

	const auto pe = tryWith(
		item
	).orElse(
		yield('!')
	);
	#endif
}