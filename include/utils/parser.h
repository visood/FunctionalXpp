#pragma once
#include "util.h"
#include <list>
#include <regex>
#include <algorithm>
#include <stdexcept>
#include <clocale>

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

//we define Parseq akin to Parser
template <typename Next, typename... Args>
using Parseq = std::function<
	ParsedResult< std::tuple<Args..., Next> > (ParsedResult<std::tuple<Args...>)
>;
template <typename Next, typename First>
using Parseq = std::function<
	ParsedResult< std::tuple<First, Next> > (ParsedResult<First>)
>;

//successful and failed results
template<typename T>
ParsedResult<T> result(const T t, const String& s)
{
	return ParsedResult<T>(t, s);
}

template<typename T>
const ParsedResult<T> nothing = ParsedResult<T>();

template <typename T>
ParsedResult<T> parse(const Parser<T>& pt, const String& in)
{
	return pt(in);
}

//a parser that returns the input string as the parsed result
const Parser<String> verbatim = Parser<String>(
	[=] (const String& in) {
		return ParsedResult<String>(in, "");
	}
);
//two simple parsers
//a parser that will always return a provided value
template<typename T>
Parser<T> yield(const T& t)
{
	return Parser<T> (
		[=] (const String& in) -> ParsedResult<T> {
			return ParsedResult<T>(t, in);
		}
	);
}
//and one that will always fail
//use it as failure<T>();
#if 0
template<typename T>
Parser<T> failure()
{
	return Parser<T> (
		[=] (const String& in) -> ParsedResult<T> {
			in + "suppress warning";
			//return ParsedResult<T>();
			return nothing<T>;
		}
	);
}
#endif 
//this is ugly.
//can we have templated variables?
template<typename T>
const Parser<T> failure = Parser<T> (
	[=] (const String& in) -> ParsedResult<T> {
		in + "suppress warning";
		return ParsedResult<T>();
	}
);

//sequence parsers with operators
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

Parser<char> item (
	[] (const String& s) -> ParsedResult<char> {
		if (s.empty()) return nothing<char>;
		return result(s[0], s.substr(1));
	}
);

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


#if 0
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

	template <typename T>
	Parser<T> collect(std::function< T(Args...) > f ) {
		return Parser<T> (
			[=] (const String& in) -> ParsedResult<T> {
				const auto rq = parse(_parser, in);
				if (rq.empty) return nothing<T>;
				return result(f(rq.value), rq.out);
			}
		);
	}

	const Parser<Ptype>& parser() const {return _parser;}

private:
	const Parser<Ptype> _parser;
}; /* class Parseq */

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
#endif

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

//a parser that satisfies a provided condition
Parser<char> sat(const std::function<bool(const char c)>& f)
{
	return item >>= bind<char> (
		[=] (const char c) -> Parser<char> {
			if (not f(c)) return failure<char>;
			return yield(c);
		}
	);
}

//parsers that sat allows us to express
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

const auto whitespace = space | nline;

const auto achar = [] (const char c) -> Parser<char> {
	return  sat (
		[c] (const char x) -> bool {
			return c == x;
		}
	);
};


#if 0
const auto astring = [] (const String& str) -> Parser<String> {
};
#endif


//put the pieces together to build higher order parsers
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

const auto replace(const String& pattern, const String& replacement)
{
	return [=] (const String& in) -> String {
		auto l = pattern.size();
		String out;
		size_t pos0 = 0;
		auto pos = in.find(pattern, pos0);
		while (pos != std::string::npos) {
			out = out + in.substr(pos0, pos - pos0) + replacement;
			pos0 = pos + l;
			pos = in.find(pattern, pos0);
		}
		return out + in.substr(pos0);
	};
}

const auto replace(const char p, const char r)
{
	return [=] (const String& in) -> String {
		size_t pos0 = 0;
		String out;
		auto pos = in.find(p, pos0);
		while (pos != std::string::npos) {
			out = out + in.substr(pos0, pos - pos0) + r;
			pos0 = pos + 1;
			pos = in.find(p, pos0);
		}
		return out + in.substr(pos0);
	};
}

const auto drop(const char c)
{
	return [=] (const String& in) -> String {
		size_t pos0 = 0;
		String out;
		auto pos = in.find(c, pos0);
		while (pos != std::string::npos) {
			out = out + in.substr(pos0, pos - pos0);
			pos0 = pos + 1;
			pos = in.find(c, pos0);
		}
		return out + in.substr(pos0);
	};
}

const auto drop(const String s)
{
	return [=] (const String& in) -> String {
		size_t pos0 = 0;
		size_t l = s.size();
		String out;
		auto pos = in.find(s, pos0);
		while (pos != std::string::npos) {
			out = out + in.substr(pos0, pos - pos0);
			pos0 = pos + l;
			pos = in.find(s, pos0);
		}
		return out + in.substr(pos0);
	};
}

//words are defined to be demarcated by spaces
//we need to strip starting spaces
auto const strip = [=] (const char c) -> Parser<String>
{
	//return several( achar(c) ) > yield(String());
	return Parser<String> (
		[=] (const String& in) -> ParsedResult<String> {
			size_t pos = 0;
			while(in[pos] == c) pos++;
			return result( String(), in.substr(pos) );
		}
	);
};

//sequence functions ---
//to work, g's input should be the output of f
//not expressible through C++ auto that we use here.
auto operator >= (const auto& f, const auto& g)
{
	return [=] (const auto& x) {
		return g(f(x));
	};
};

const auto unpunctuated = (
	drop(',') >=
	drop('.') >=
	drop(';') >=
	drop(':') >=
	drop('\n') >=
	drop('\t') >=
	drop('-')
);

const auto sentence = strip(' ') > split('.');
const auto sentences = many( sentence );
//punctuation can be bad

Parser<String> wordfun(
	item >>= bind<char> (
		[=] (const char c) -> Parser<String> {
			if (c == ' ' or c == '\n') return yield(String());
			return yield(String() + c) + wordfun;
		}
	)
);

const Parser<String> word0 = (
	strip(' ') > (
		split(' ') |
		split(',') |
		split('.')
	)
);

const Parser<String> word = word0 >>= bind<String> (
	[=] (const String& w) -> Parser<String> {
		return yield(unpunctuated(w));
	}
);

//functional programming allows us to express what a parser string
//elegantly, and transparently
//an imperative, and faster implementation will be
//to loop over query string s, and match the characters against
//the input to the parser. or in == s.substr(0, in.size())
//this will still require use to provide wrapping Parser<...> 
const Parser<String> astring( const String& s)
{
	if (s.size() == 0) return yield(String());
	return achar(s[0]) > astring(s.substr(1)) > yield(s);
}

const auto istring(const String& s)
{
	if (s.size() == 0) return yield(String());
	return Parser<String> (
		[=] (const String& in) -> ParsedResult<String> {
			if ( s == in.substr(0, s.size()) )
				return result(s, in.substr(s.size()));
			return nothing<String>;
		}
	);
}


//some more primitives
const auto lower = sat(
	[=] (const char c) {
		return std::islower(c);
	}
);
const auto upper = sat(
	[=] (const char c) {
		return std::isupper(c);
	}
);

//we can obtain the result of a Parser
#if 0
template <typename T>
auto operator <<=(ParsedResult<T>* x, const Parser<T>& pt)
{
	return [x, pt] (const String& in) {
		std::cout << "now run it " << std::endl;
		const auto r = parse(pt, in);
		std::cout << "result " << r.value << " rest "  << r.out << std::endl;
		x->value = r.value;
		x->out = r.out;
	};
}
#endif
//we will need to convert a list of chars to a string

String to_string(const List<char>& cs)
{
	return std::accumulate(begin(cs), end(cs), String());
}

//an identifier must start with a lower case
#if 0
const Parser<String> ident = lower >>= bind<char> (
	[=] (const char x) -> Parser<String> {
		return many(alphanum) >>= bind< List<char> > (
			[=] (const List<char>& xs) -> Parser<String> {
				return yield(to_string(x >= xs));
			}
		);
	}
);
//ideally we would like to write a composite Parser like this
const Parser<String> ident = {
	const auto x <<= lower;
	const auto xs <<= many(alphanum);
	yield(to_string(x >= xs));
}
//not valid C++, we try
const Parser<String> ident = sequence(
	lower
).followed_by(
	many(alphanum)
).collect(
	[=] (const char c, const List<char>& cs) {
		return c >= cs;
	}
);
//or more simply

const Parser<String> ident = Parseq<char, List<char>>(
	lower & many(alphanum)
).collect (
	[=] (const char c, const List<char>& cs) {
		return c >= cs;
	}
);
//or we can use operators
const Parser<String> ident = (lower & many(alphanum)) >>= (
	[=] (const auto x, const auto& xs) {
		return x >= xs;
	}
)

#endif


//the type Parseq
Parseq<String, String> twoWords(
	[=] (const ParsedResult<String>& r1) -> Parseq<String, String> {
		if (r1.empty) return nothing< std::tuple<String, String> >;
		const auto r2 = parse(word, r1.out);
		if (r2.empty) return nothing< std::tuple<String, String> >;
		return result(std::make_tuple(r1.value, r2.value), r2.out);
	}
);
namespace Regex
{
// we use regex to parse, in association of our functional parser
} /* namespace Regex */

} /* namespace Expression */
