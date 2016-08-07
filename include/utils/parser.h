//a functional monadic parser
//we will implement both Scala and Haskell style monad here

#pragma once
#include "monadic.h"
#include <regex>
#include <algorithm>
#include <stdexcept>
#include <clocale>
#include <regex>

namespace Expression
{
//using namespace Monadic;
using String = std::string;

//a const list can be a good example to design an immutable,
//persistent data structure in C++.
// a wrapper around std::list that employs monadic relationships
template <typename T>
using List = std::list<T>;
//we do not want to create a wrapper class, instead we use operators
template< typename T>
List<T> unit(const T& t) { return List<T>(t);}

//the list should be cons list, we will use the stream operator
//this is not the ideal solution
//ideally we would like >> to create a new (persistent, immutable) list
//checkout implementation attempt in list.h
template<typename T>
List<T>& operator >> (const T& head, List<T>& tail)
{
	tail.push_front(head);
	return tail;
}

template<typename T>
const List<T>& operator >> (const T& head, List<T> tail)
{
	tail.push_front(head);
	return std::move(tail);
}

template<typename T>
const T& head(const List<T>& list) { return *begin(list);}

template<typename T>
List<T> tail(const List<T>& list) { return List<T>(++begin(list), end(list));}



//for a list monad we need to flatten a list of lists
template<typename T>
List<T> flatten(List< List<T> >& llt)
{
	List<T> ltout;
	auto it = ltout.begin();
	for (const auto& lt : llt) {
		ltout.insert(it, lt);
	}
	return ltout;
}
//an explicit map
template<
	typename T,
	typename F,
	typename S = typename std::result_of<F&(T)>::type
>
inline List<S> map(const F f, const List<T>& ts)
{
	List<S> ss;
	for (const auto& t : ts) ss.push_back(f(t));
	return ts;
}
//bind
template<
	typename T,
	typename F,
	typename S = typename std::result_of<F&(T)>::value_type
>
inline List<S>& operator >>= (
	List<T>& ts,
	const F& fst
)
{
	return flatten( map(fst, ts));
}

template<typename T>
List<T> nil = List<T>();

template<typename T>
bool operator==(const List<T>& l1, const List<T>& l2)
{
	if (l1.empty()) return l2.empty();
	if (l2.empty()) return false;
	return head(l1) == head(l2) and tail(l1) == tail(l2);
}

template<typename T>
void print(const List<T>& l)
{
	if (l.empty()) {
		std::cout << std::endl;
		return;
	}
	std::cout << head(l) << ", ";
	print(tail(l));
}

template<typename T>
String to_string(const List<T>& l)
{
	if (l.empty())	return String();
	return std::to_string(head(l)) + to_string(tail(l));
}



//to store the success of a parser
template<typename T>
struct ParsedResult
{
	using type = T;
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
	String out;
	bool empty;
};

//utility functions to construct ParsedSuccesss
template<typename T>
ParsedResult<T> some(const T& t, const String& s)
{
	return ParsedResult<T>(t, s);
}

template<typename T>
const ParsedResult<T> empty = ParsedResult<T>();

//the parser itself is a function
template< typename T >
using Parser = std::function< ParsedResult<T>(String) >;
//having a type defined for the Parser will make the code
//more expressive, though we will provide facility to use
//plain lambdas as Parsers ... we will simply need template code
//to deduce the types
//because both std::function and lambdas are basically functions,
//we will be able to mix them when we sequence parsers

//a utility function to parse
template< typename T>
ParsedResult<T> parse(const Parser<T>& pt, const String& in)
{
	return pt(in);
}
//that can also be used with autos
auto parse(const auto& pt, const String& in)
{
	return pt(in);
}

//a parser that always fails
template<typename T>
const Parser<T> failure = Parser<T>([=] (const String& in) {
		in + "suppress unused variable warning";
		return empty<T>;
	}
);

//the parser itself is a monad
//that means the functionality associated with a monad
//monadic return.
//this could be a plain old C++ function, or we use a lambda
//with auto type deduction
//we will use one of these, others are left here for documentation
template< typename T >
const Parser<T> yield(const T& t)
{
	return Parser<T> ( [=] (const String& in) -> ParsedResult<T> {
			return ParsedResult<T> (t, in);
		}
	);
}
//needs default constructor
template< typename T >
const Parser<T> yield() { return yield(T()); }
#if 0
template<typename T>
const auto yield( const T& t)
{
	return [=] (const String& in) {
		return some(t, in);
	};
}
const auto yield = [] (const auto x) {
	return [=] (const String& in) {
		return some(x, in);
	};
};
#endif


//an operator to bind
template< typename T, typename S >
Parser<S> operator >>= (
	const Parser<T>& pt,
	const std::function< Parser<S>(T) >& fst
)
{
	return Parser<S> ( [=] (const String& in) {
			const auto rt = parse(pt, in);
			if (rt.empty) return empty<S>;
			return parse(fst(rt.value), rt.out);
		}
	);
}

//we will not like to pass std::function to use the bind operator >>=
template<
	typename T,
	typename F,
	typename PS = typename std::result_of<F&(T)>::type,
	typename RS = typename std::result_of<PS&(String)>::type,
	typename S = typename RS::type
>
Parser<S> operator >>= (
	const Parser<T>& pt,
	const F& fst
)
{
	return Parser<S> ( [=] (const String& in) {
			const auto rt = parse(pt, in);
			if (rt.empty) return empty<S>;
			return parse(fst(rt.value), rt.out);
		}
	);
}

//we need to deduce more types if the parser is also a lambda
template<
	typename PT,
	typename F,
	typename RT = typename std::result_of<PT&(String)>::type,
	typename T = typename RT::type,
	typename PS = typename std::result_of<F&(T)>::type,
	typename RS = typename std::result_of<PS&(String)>::type,
	typename S = typename RS::type
>
const Parser<S> operator >>= (
	const PT& pt,
	const F& fst
)
{
	return [=] (const String& in) {
		const auto rt = pt(in);
		if (rt.empty) return empty<S>;
		return fst(rt.value)(rt.out);
	};
}

//we also need to bind when the return of the first parser is unused
//since we will drop the result of the first parser,
//we do not need to its returned type, and can simply auto it
template<typename S>
Parser<S> operator >> (
	const auto& pt,
	const Parser<S>& ps
)
{
	return Parser<S>([=] (const String& in) {
			const auto rt = parse(pt, in);
			if (rt.empty) return empty<S>;
			return parse(ps, rt.out);
		}
	);
}
//and we will have to work harder when the second parser is a lambda
template<
	typename PS,
	typename RS = typename std::result_of<PS&(String)>::type,
	typename S = typename RS::type
>
Parser<S> operator >> (
	const auto& pt,
	const auto& ps
)
{
	return Parser<S>( [=] (const String& in) {
			const auto rt = parse(pt, in);
			if (rt.empty) return empty<S>;
			return parse(ps, rt.out);
		}
	);
}

//we can also define an operator that can be used to chain
//to obtain a std::tuple as the chained output

template<typename T, typename S>
Parser<std::tuple<T, S> > operator & (
	const Parser<T>& pt,
	const Parser<S>& ps
)
{
	return pt >>= [=] (const T& t) {
		return ps >>= [=] (const S& s) {
			return yield(std::make_tuple(t, s));
		};
	};
}

template<typename Next, typename... This>
auto operator &(
	const Parser< std::tuple<This...> >& pt,
	const Parser<Next>& pn
)
{
	return pt >>= [=] (const std::tuple<This...>& ts) {
		return pn >>= [=] (const Next& n) {
			return yield(std::tuple_cat(ts, std::make_tuple(n)));
		};
	};
}


//some primitive and compound parsers to utilize
//we may use the wrapped lambdas (as Parser)
const auto item =  Parser<char> ([] (const String& s) {
		if (s.empty()) return empty<char>;
		return some(s[0], s.substr(1));
	}
);

//we would like this to be shorter, but function templates
//cannot be specialized
template<typename T, size_t N>
struct repeatImpl
{
	auto from(const Parser<T>& pt)
	{
		return pt >>= [=] (const auto& t) {
			return repeatImpl<T, N-1>().from(pt) >>= (
				[=] (const auto& ts) {
					return yield(
						std::tuple_cat(std::make_tuple(t), ts)
					);
				}
			);
		};
	}
};
template<typename T>
struct repeatImpl<T, 1>
{
	auto from(const Parser<T>& pt)
	{
		return pt >>= [=] (const auto& t) {
			return yield(std::make_tuple(t));
		};
	}
};
template<size_t N, typename T>
const auto repeat(const Parser<T>& pt)
{
	return repeatImpl<T, N>().from(pt);
}

//a parser may fail, then we might want to run another one
template<typename T>
Parser<T> operator | (
	const Parser<T>& pt1,
	const Parser<T>& pt2
)
{
	return Parser<T> ( [=] (const String& in) {
			const auto rt1 = parse(pt1, in);
			if (rt1.empty) return parse(pt2, in);
			return rt1;
		}
	);
}


//many will be slow if it has to parse a large number of items
//passing non-const reference causes incomprehensible compile errors
//use imperative several instead
template<typename T>
Parser< List<T> > many(const Parser<T>& pt)
{
	return many1(pt) | yield(List<T>());
}
template<typename T>
Parser< List<T> > many1(const Parser<T>& pt)
{
	return pt >>= [=] (const T& t) {
		return many(pt) >>= [=] (const List<T>& ts) {
			return yield(t >> ts);
		};
	};
}
#if 0
//imperative version of many
template<typename T>
Parser< List<T> > several(const Parser<T>& pt)
{
	return Parser< List<T> > ([=] (const String& in) {
			List<T> lt;
			String finalOut = in;
			auto rt = parse(pt, in);
			while (not rt.empty) {
				lt.push_back(rt.value);
				finalOut = rt.out;
				rt = parse(pt, rt.out);
			}
			return some(lt, finalOut);
		}
	);
}
#endif

//we can generalize several to use a lambda to accumulate results
template<
	typename T,
	typename L,
	typename F
>
Parser<L> accumulate(const F& f, const Parser<T>& pt, const L& l)
{
	return Parser<L> (
		[=] (const String& in) {
			String finalOut = in;
			auto rt = parse(pt, in);
			L lmr = l;
			while (not rt.empty) {
				lmr = f(lmr, rt.value);
				finalOut = rt.out;
				rt = parse(pt, rt.out);
			}
			return some(lmr, finalOut);
		}
	);
}
//accumulate implies some kind of sum,
//however if the output is a list,
//we are putting the results into another container
//infact we are changing the monadic category
template<
	typename M,
	typename T,
	typename F
>
Parser<M> mmap(const F& f, const Parser<T>& pt, const M& m)
{
	return Parser<M> (
		[&] (const String& in) {
			String finalOut = in;
			auto rt = parse(pt, in);
			M mc = m;
			while (not rt.empty) {
				f(mc, rt.value);
				finalOut = rt.out;
				rt = parse(pt, rt.out);
			}
			return some(mc, finalOut);
		}
	);
}
template<
	typename M,
	typename T,
	typename F
>
Parser<M> mmap(const F& f, const Parser<T>& pt)
{
	return mmap(f, pt, M());
}
template<typename T>
Parser< List<T> > several(const Parser<T>& pt)
{
	return mmap< List<T> >(
		[=] (List<T>& l, const T& t) {
			l.push_back(t);
		},
		pt
	);
}

template<typename T>
Parser<uint> freq(const Parser<T>& pt)
{
	return accumulate(
		[=] (const uint s, const uint t) -> uint {
			return s + t;
		},
		pt >> yield(1U), 0U
	);
}

//a type to parse repeated occurrences of the same pattern
template<typename T>
using Freq = std::pair<T, uint>;

//many parsers can be described in terms of a sat
Parser<char> sat(const auto& pred)
{
	return item >>= [=] (const char c) {
		if (not pred(c)) return failure<char>;
		return yield(c);
	};
}

const auto isSpace = [=] (const char c) { return c == ' ';};

const auto char_(const char c)
{
	return sat([=] (const char x) {return c == x;});
}

const auto space = many(sat(isSpace)) >> yield<String>();

//const auto space = char_(' ');
const auto nline = char_('\n');
const auto tab = char_('\t');
const auto whitespace = char_(' ') | nline | tab;
const auto period = char_('.');

//remove space
template <typename T>
Parser<T> token(const Parser<T>& pt)
{
	return (
		space >>
		pt >>= [=] (const T& t) {
			return space >> yield(t);
		}
	);
}


//regex enabled parsers
Parser<String> capture(const String& pattern)
{
	return Parser<String> (
		[=] (const String& in) {
			std::smatch matches;
			std::regex mexp( String("(") + pattern + ")(.*)");
			if (std::regex_match(in, matches, mexp))
				return some(matches[1].str(), matches[2].str());
			return empty<String>;
		}
							
	);
}

}
/* namespace Expression */
