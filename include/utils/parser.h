#pragma once
#include "util.h"
#include <list>
#include <regex>
#include <algorithm>
#include <stdexcept>

namespace Expression
{

using String = std::string;

template <typename T>
using List = std::list<T>;

template<typename T>
struct ParsedResult
{
public:
	ParsedResult(const T t, const String& s) :
		value(t),
		out(s),
		empty(false)
	{}
	ParsedResult() :
		value(T()),
		out(""),
		empty(true)
	{}


	T value;
	String out = "";
	bool empty = true;
};

template <typename T>
using Parser = std::function<ParsedResult<T>(String)>;

template <typename T>
ParsedResult<T> parse(const Parser<T>& pt, const String& in)
{
	return pt(in);
}

//parse with one parser, and the remaining string with another,
//and combine the results using a function
template <typename T, typename S>
Parser<S> operator >>= (
	const Parser<T> pt,
	const std::function<Parser<S>(T)>& fps
) {
	return Parser<S> (
		[=] (const String& in) -> ParsedResult<S> {
			const ParsedResult<T> rt = parse(pt, in);
			if (rt.empty) return ParsedResult<S>();
			return parse(fps(rt.value), rt.out);
		}
	);
}

//sometimes we will need to ignore the value parsed,
//and use the out string as input to the next parser.
template <typename T, typename S>
Parser<S> operator > (
	const Parser<T>& pt,
	const Parser<S>& ps
) {
	return Parser<S> (
		[=] (const String& in) -> ParsedResult<S> {
			const auto rt = parse(pt, in);
			if (rt.empty) return ParsedResult<S>();
			return parse(ps, rt.out);
		}
	);
}

template <
	typename T,
	typename F,
	typename R = typename std::result_of<F&(T)>::type
	> std::function<R(T)> bind(F f)
{
	return std::function<R(T)>(f);
}

template<typename T>
Parser<T> failure()
{
	return Parser<T> (
		[=] (const String& in) -> ParsedResult<T> {
			in + "suppress warning";
			return ParsedResult<T>();
		}
	);
}

template<typename T>
Parser<T> yield(const T& t)
{
	return Parser<T> (
		[=] (const String& in) -> ParsedResult<T> {
			return ParsedResult<T>(t, in);
		}
	);
}

Parser<char> item (
	[] (const String& s) -> ParsedResult<char> {
		if (s.empty()) return ParsedResult<char>();
		return ParsedResult<char>(s[0], s.substr(1));
	}
);

Parser<char> sat(const std::function<bool(const char c)>& f)
{
	return item >>= bind<char> (
		[=] (const char c) -> Parser<char> {
			if (not f(c)) return failure<char>();
			return yield(c);
		}
	);
}

const auto space = sat (
	[](const char c) -> bool {
		return c == ' ';
	}
);

const auto nline = sat (
	[](const char c) -> bool {
		return c == '\n';
	}
);

const auto whitespace = sat (
	[](const char c) -> bool {
		return space(c) or nline(c);
	}
);

const auto achar = [] (const char c) -> Parser<char> {
	return  sat (
		[c] (const char x) -> bool {
			return c == x;
		}
	);
};


const auto astring = [] (const String& str) -> Parser<String> {
	
};


template<typename T, typename S>
Parser<std::tuple<T, S> > operator & (
	const Parser<T>& pt,
	const Parser<S>& ps
) {
	using Extup = std::tuple<T, S>;
	return Parser<Extup> (
		[=] (const String& in) -> ParsedResult<Extup> {
			const auto rt = parse(pt, in);
			if (rt.empty) return ParsedResult<Extup>();
			const auto rs = parse(ps, rt.out);
			if (rs.empty) return ParsedResult<Extup>();
			return ParsedResult<Extup>(
				std::make_tuple(rt.value, rs.value),
				rs.out
			);
		}
	);
}

template<typename Next, typename... This>
Parser< std::tuple<This..., Next> > operator & (
	const Parser<std::tuple<This...> >& pthis,
	const Parser<Next>& pnext
) {
	using Extup = std::tuple<This..., Next>;
	return Parser<Extup> (
		[=] (const String& in) -> ParsedResult<Extup> {
			const auto rthis = parse(pthis, in);
			if (rthis.empty) return ParsedResult<Extup>();
			const auto rnext = parse(pnext, rthis.out);
			if (rnext.empty) return ParsedResult<Extup>();
			return ParsedResult<Extup>(
				std::tuple_cat(
					rthis.value,
					std::make_tuple(rnext.value)
				),
				rnext.out
			);
		}
	);
}

template<size_t N, typename T>
auto repeat(const Parser<T>& p, int_<N>)
{
	return repeat(p, int_<N-1>()) & p;
}

template<typename T>
auto repeat(const Parser<T>& p, int_<2>)
{
	return p & p;
}

//we may extend repeat logically,
//to include the case of a single occurrence
template<typename T>
Parser<std::tuple<T> > repeat(const Parser<T>& p, int_<1>)
{
	using Ptype = std::tuple<T>;
	return p >>= bind<T> (
		[=] (const auto& t) -> Parser<Ptype> {
			return yield(std::make_tuple(t));
		}
	);
}

template<typename T>
List<T>& operator >= (const T& head, List<T>& tail)
{
	tail.push_front(head);
	return tail;
}

template<typename T>
const List<T> operator >= (const T& head, List<T> tail)
{
	tail.push_front(head);
	return tail;
}

template<typename T>
T head(const List<T>& list)
{
	return *begin(list);
}

template<typename T>
List<T> tail(const List<T>& list)
{
	return List<T>(++begin(list), end(list));
}

template<typename T>
List<T> nil()
{
	return {};
}

//a functional parser that parses many Parser<T>
//coded using mutual recursion
//a parser that parses several values of pt must not fail
template<typename T>
Parser<List<T>> many(const Parser<T>& pt)
{
	return many1(pt) | yield(List<T>());
}
template<typename T>
Parser<List<T> > many1(const Parser<T>& pt)
{
	using Ptype = List<T>;
	return pt >>= bind<T> (
		[=] (const auto& t) -> Parser<Ptype> {
			return many(pt) >>= bind<Ptype> (
				[=] (const auto& ts) -> Parser<Ptype> {
					return yield(t >= ts);
				}
			);
		}
	);
}

//an imperative many parser
//clearly not as elegant as the functional version above
//a parser that parses several values of pt must not fail
template<typename T>
Parser< List<T> > several(const Parser<T>& pt)
{
	using Ptype = List<T>;
	return Parser< Ptype > (
		[=] (const String& in) -> ParsedResult< Ptype >  {
			List<T> lt; //an empty list
			String finalOut = in;
			auto rt = parse(pt, in);
			while (not rt.empty) {
				lt.push_back(rt.value);
				finalOut = rt.out;
				rt = parse(pt, rt.out);
			}
			return ParsedResult< Ptype >(lt, finalOut);
		}
	);
}

//a type to parse repeated occurrences of the same pattern
template<typename T>
using Freq = std::pair<T, uint>;

//how many consecutive occurrences of a Parser<T> ?
template<typename T>
Parser<uint> freq(const Parser<T>& pt)
{
	return Parser<uint> (
		[=] (const String& in) -> ParsedResult<uint> {
			uint n = 0;
			String finalOut = in;
			auto rt = parse(pt, in);
			while(not rt.empty) {
				n += 1;
				finalOut = rt.out;
				rt = parse(pt, rt.out);
			}
			return ParsedResult< uint > (n, finalOut);
		}
	);
}


template <typename... Args>
class Parseq
{
	using Ptype = std::tuple<Args...>;
public:
	Parseq(const Parser<Ptype>& pt) :
		_parser(pt)
	{}

	template<typename T>
	Parseq<Args..., T> followed_by(const Parser<T>& pt) const
	{
		return Parseq<Args..., T>( _parser & pt );
	}

	const Parser<Ptype>& parser() const {return _parser;}

private:
	const Parser<Ptype> _parser;
};

template <typename T>
Parseq<T> parseq(const Parser<T>& pt)
{
	using TupT = std::tuple<T>;
	return Parseq<T> (
		[=] (const String& in) -> ParsedResult<TupT> {
			const auto r = parse(pt, in);
			if (r.empty) return ParsedResult<TupT>();
			return ParsedResult<TupT>(std::make_tuple(r.value), r.out);
		}
	);
}

template<typename... Args>
ParsedResult<std::tuple<Args...> > parse(
	const Parseq<Args...>& pqt,
	const String& in
){
	return parse(pqt.parser(), in);
}

template<typename T>
Parser<T> operator | (const Parser<T>& pt, const Parser<T>& pe )
{
	return Parser<T> (
		[=] (const String& in) -> ParsedResult<T> {
			const auto rt = parse(pt, in);
			if (rt.empty) return parse(pe, in);
			return parse(pt, in);
		}
	);
}

template<typename T>
Parser<T> operator +(const Parser<T> left, const Parser<T> right)
{
	return Parser<T> (
		[=] (const String& in) -> ParsedResult<T> {
			const auto rleft = parse(left, in);
			if (rleft.empty) return ParsedResult<T>();
			const auto rright = parse(right, rleft.out);
			if (rright.empty) return rleft;
			return ParsedResult<T>(rleft.value + rright.value, rright.out);
		}
	);
}

Parser<String> wordfun(
	item >>= bind<char> (
		[=] (const char c) -> Parser<String> {
			if (c == ' ' or c == '\n') return yield(String());
			return yield(String() + c) + wordfun;
		}
	)
);

//recursion is slow in c++,
//so we write a parser that uses a while loop
//words are defined to be demarcated by spaces
//we need to strip surrounding spaces
Parser<String> strip(const char sep = ' ')
{
	return many(sep) >>= bind< List<char> > (
		[=] (const auto& l) -> Parser<String> {
			return Parser<String> (
				[=] (const String& in) -> ParsedResult<String> {
					const auto rsp = parse(split(sep), in);
					if (rsp.empty) return ParsedResult<String>();

				}
			);
		}
	);
}

Parser<String> split(const char sep)
{
	return Parser<String> (
		[=] (const String& in) {
			const auto p = in.find(sep);
			if (p == std::string::npos) return ParsedResult<String>();
			return ParsedResult<String>(in.substr(0, p), in.substr(p+1));
		}
	);
}

const Parser<String> word = split(' ') | split('.');
#if 0
Parser<String> word (
	[=] (const String& in) -> ParsedResult<String> {
		const auto p = in.find(' ');
		if (p == std::string::npos) return ParsedResult<String>();
		return ParsedResult<String>(in.substr(0, p), in.substr(p+1));
	}
);
#endif


namespace Regex
{
// we use regex to parse, in association of our functional parser
} /* namespace Regex */

} /* namespace Expression */
