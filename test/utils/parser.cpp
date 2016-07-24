#include "catch.hpp"
#include "parser.h"

using namespace Expression;

TEST_CASE("Functional Parser basics", "[FunctionalParserBasics]")
{
	const String hw("hello world bonjour tous monde namastey jee");
	ParsedResult<char> r = parse(item, hw);
	do {
		r = parse(item, r.out);
		std::cout << "parse item " << r.value << std::endl;
	} while (not r.empty);


	const Parser<char> ish = sat(
		[] (const char c) {return c == 'h';}
	);

	const auto rh = parse(ish, hw);
	std::cout << "sat of h " << rh.value << ", " << rh.out << std::endl;

	const auto rw = parse(word, hw);
	std::cout << "a word: " << rw.value << ", " << rw.out << std::endl;


	//sequence of parsers
	const auto twoWords = word & word;
	const auto r2w = parse(twoWords, hw);
	std::cout << "parse two words from " << hw << ": "
						<< std::get<0>(r2w.value) << ", "
						<< std::get<1>(r2w.value) << std::endl;

	const auto threeWords = word & word & word;
	const auto r3w = parse(threeWords, hw);
	std::cout << "parse three words from " << hw << ": "
						<< std::get<0>(r3w.value) << ", "
						<< std::get<1>(r3w.value) << ", "
						<< std::get<2>(r3w.value) << std::endl;

	const auto sevenWords = (
		threeWords & word & word & word & word
	);

	const auto r7w = parse(sevenWords, hw);
	std::cout << "parse three words from " << hw << ": "
						<< std::get<0>(r7w.value) << ", "
						<< std::get<1>(r7w.value) << ", "
						<< std::get<2>(r7w.value) << ", "
						<< std::get<3>(r7w.value) << ", "
						<< std::get<4>(r7w.value) << ", "
						<< std::get<5>(r7w.value) << ", "
						<< std::get<6>(r7w.value) << std::endl;


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
