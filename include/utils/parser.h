//a functional monadic parser
//we will implement both Scala and Haskell style monad here

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
template<typename T>
using List = std::list<T>;
//a head/tail list
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

	const T value;
	const String out;
	const bool empty;
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
			return yield(t >= ts);
		};
	};
}


//many parsers can be described in terms of a sat
Parser<char> sat(const auto& pred)
{
	return item >>= [=] (const char c) {
		if (not pred(c)) return failure<char>;
		return yield(c);
	};
}

const auto char_(const char c)
{
	return sat([=] (const char x) {return c == x;});
}

const auto space = char_(' ');
const auto nline = char_('\n');
const auto tab = char_('\t');
const auto whitespace = space | nline | tab;
const auto period = char_('.');

}
/* namespace Expression */
