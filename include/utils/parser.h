//a functional monadic parser
//we will implement both Scala and Haskell style monad here

#pragma once
#include "monadic.h"
#include <regex>
#include <algorithm>
#include <stdexcept>
#include <clocale>
#include <regex>
#include "stdwrapped.h"


//macros for nicer parser syntax

#define return_(x) return yield(x)


using namespace std::wrapped;

namespace Expression
{


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

//a template specialization when there is no value!
template<>
struct ParsedResult<void>
{
	using type = void;
	ParsedResult(const String& s) :
		out(s),
		empty(false)
	{}
	ParsedResult() :
		out(""),
		empty(true)
	{}
	String out;
	bool empty;
};

//utility functions to construct ParsedResult
template<typename T>
ParsedResult<T> some(const T& t, const String& s)
{
	return ParsedResult<T>(t, s);
}

//i cannot specialize to template<>, compile errors
//but i can overload
ParsedResult<void> some(const String& s)
{
	return ParsedResult<void>(s);
}

template<typename T>
const ParsedResult<T> empty = ParsedResult<T>();

template<>
const ParsedResult<void> empty<void> = ParsedResult<void>();

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
inline ParsedResult<T> parse(const Parser<T>& pt, const String& in)
{
	return pt(in);
}
//that can also be used with autos
inline auto parse(const auto& pt, const String& in)
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
inline const Parser<T> yield(const T& t)
{
	return Parser<T> ( [=] (const String& in) -> ParsedResult<T> {
			return ParsedResult<T> (t, in);
		}
	);
}
//needs default constructor
//template< >
const Parser<void> yield() {
	return Parser<void> ( [=] (const String& in) {
			return some(in);
		}
	);
}

#if 0
template<typename T>
inline const auto yield( const T& t)
{
	return [=] (const String& in) {
		return some(t, in);
	};
}
inline const auto yield = [] (const auto x) {
	return [=] (const String& in) {
		return some(x, in);
	};
};
#endif


//an operator to bind
template< typename T, typename S >
inline Parser<S> operator >>= (
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

//that we need to define for a Parser<void> as well
template<typename S>
inline Parser<S> operator >>= (
	const Parser<void>& pt,
	const std::function< Parser<S>() >& fs
)
{
	return Parser<S> ( [=] (const String& in) {
			const auto rt = parse(pt, in);
			if (rt.empty) return empty<S>;
			return parse(fs(), rt.out);
		}
	);
}


//we will not like to pass std::function to use the bind operator >>=
template<
	typename T,
	typename F,
	typename PS = typename std::result_of<F&(T)>::type,
	typename RS = typename std::result_of<PS&(String)>::type,
	typename S  = typename RS::type
>
inline Parser<S> operator >>= (
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
//lambda bind function for Parser<void>
template<
	typename F,
	typename PS = typename std::result_of<F&()>::type,
	typename RS = typename std::result_of<PS&(String)>::type,
	typename S  = typename RS::type
>
inline Parser<S> operator >>= (
	const Parser<void>& pt,
	const F& fs
)
{
	return Parser<S> ( [=] (const String& in) {
			const auto rt = parse(pt, in);
			if (rt.empty) return empty<S>;
			return parse(fs(), rt.out);
		}
	);
}
//we need to deduce more types if the parser is also a lambda
template<
	typename PT,
	typename F,
	typename RT = typename std::result_of<PT&(String)>::type,
	typename T  = typename RT::type,
	typename PS = typename std::result_of<F&(T)>::type,
	typename RS = typename std::result_of<PS&(String)>::type,
	typename S  = typename RS::type
>
inline const Parser<S> operator >>= (
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
//lambda bind function for a lambda that implies a Parser of voids
template<
	typename PT,
	typename F,
	typename PS = typename std::result_of<F&()>::type,
	typename RS = typename std::result_of<PS&(String)>::type,
	typename S  = typename RS::type
>
inline const Parser<S> operator >>= (
	const PT& p,
	const F& fs
)
{
	return [=] (const String& in) {
		const auto r = p(in);
		if (r.empty) return empty<S>;
		return fs()(r.out);
	};
}

//we also need to bind when the return of the first parser is unused
//since we will drop the result of the first parser,
//we do not need to its returned type, and can simply auto it
template<typename S>
inline Parser<S> operator >> (
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


//our monadic interpretation of a parser is not as simple as that of a list
//so far an empty monad has contained neither a value, nor an out string.
//however we can imagine the value of non-empty monad that has no value,
//but does have an out string.
//we have implemented by returning a ParsedResult<T>(T(), out)
//in our current implementation.
//we can define a claa Nothing!
//our we can mimic the effect of such a null type through defining
//bind function >>= that takes a function with no arguments.
//this function will implicitly convert any parser on the left into
//a parser without any value type (parsed values are lost).
//plus the resulting syntax when chaining parsers is cleaner and
//clearer (we do not use >>, instead >>= goes everywhere).
//however a parser of null type can also be defined using
//a template specializing that takes no parameters,
//but our parser is not a class!
#if 0
template<
	typename F,
	typename PS = typename std::result_of<F&()>::type,
	typename RS = typename std::result_of<PS&(String)>::type,
	typename S  = typename  RS::type
>
inline Parser<S> operator >>= (
	const auto& pt,
	const F& f
)
{
	return pt >> f();
}
#endif
//and we will have to work harder when the second parser is a lambda
template<
	typename PS,
	typename RS = typename std::result_of<PS&(String)>::type,
	typename S = typename RS::type
>
inline Parser<S> operator >> (
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
inline Parser<std::tuple<T, S> > operator & (
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
//specialize to when the second Parser drops its results
template<typename T>
inline Parser< std::tuple<T> > operator & (
	const Parser<T>& pt,
	const Parser<void>& p
)
{
	return pt >>= [p] (const T& t) {return p >> yield(std::make_tuple(t));};
}

template<typename Next, typename... This>
inline auto operator &(
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
//specialize to when Next  is Parser<void>
template<typename... This>
inline auto operator &(
	const Parser< std::tuple<This...> >& pt,
	const Parser<void>& p
)
{
	return pt >>= [p] (const std::tuple<This...>& ts) {return p >> yield(ts);};
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
inline const auto repeat(const Parser<T>& pt)
{
	return repeatImpl<T, N>().from(pt);
}

//a parser may fail, then we might want to run another one
template<typename T>
inline Parser<T> operator | (
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

//and if a parser fails we may want to move on to the next one
template<typename T>
inline Parser<T> maybe(const Parser<T>& pt) {return pt | yield(T());}

inline Parser<void> maybe(const Parser<void>& p) {return p | yield();}

//if we want to drop the result of a parser, we can also do

template<typename T>
inline Parser<void> drop(const Parser<T>& pt){ return pt >> yield(); }



//many will be slow if it has to parse a large number of items
//passing non-const reference causes incomprehensible compile errors
//use imperative several instead
template<typename T>
inline Parser< List<T> > many(const Parser<T>& pt)
{
	return many1(pt) | yield(List<T>());
}

using ::operator >>;

template<typename T>
inline Parser< List<T> > many1(const Parser<T>& pt)
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
inline Parser< List<T> > several(const Parser<T>& pt)
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
	typename T, //type of the parser
	typename L, //type to accumulate in
	typename F //function to accumulate with
>
inline Parser<L> accumulate(const Parser<T>& pt, const L& l, const F& f)
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


//and if we want to accumulate into a trivial value
template<
	typename L,
	typename T,
	typename F
>
inline Parser<L> accumulate(const Parser<T>& pt, const F& f)
{
	return accumulate(pt, L(), f);
}
template<typename T>
inline Parser< List<T> > several(const Parser<T>& pt)
{
	return accumulate< List<T> >(
		pt,
		[=] (List<T>& l, const T& t) {
			l.push_back(t);
			return l;
		}
	);
}

template<typename T>
inline Parser<uint> freq(const Parser<T>& pt)
{
	return accumulate(
		pt >> yield(1U), 0U,
		[=] (const uint s, const uint t) -> uint {
			return s + t;
		}
	);
}

//a type to parse repeated occurrences of the same pattern
template<typename T>
using Freq = std::pair<T, uint>;

//many parsers can be described in terms of a sat
inline Parser<char> sat(const auto& pred)
{
	return item >>= [=] (const char c) {
		if (not pred(c)) return failure<char>;
		return yield(c);
	};
}

const auto isSpace = [=] (const char c) { return c == ' ';};

inline const auto char_(const char c)
{
	return sat([=] (const char x) {return c == x;});
}
//and we may want to drop a character
inline Parser<void> drop(const char c) {return char_(c) >> yield();}

//and sometimes drop whole strings
//and recursion makes it easier to think and define
inline Parser<void> drop(const String& s)
{
	return s.size() == 0 ? yield() : char_(s[0]) >> drop(s.substr(1));
	/*
	if (s.size() == 0) return yield();
	return char_(s[0]) >> drop(s.substr(1));
	*/
}

const auto space = many(sat(isSpace)) >> yield();

//const auto space = char_(' ');
const auto nline = char_('\n');
const auto tab = char_('\t');
const auto whitespace = char_(' ') | nline | tab;
const auto period = char_('.');

//remove space
template <typename T>
inline Parser<T> token(const Parser<T>& pt)
{
	return space >>= [=] () {
		return pt >>= [=] (const T& t) {
			return space >>= [=] () {
				return yield(t);
			};
		};
	};
}


//regex enabled parsers
inline Parser<String> capture(const String& pattern)
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

//for nicer syntax


#if 0
template<typename R, typename T, typename... Rest>
struct Combinator
{
	Combinator(const std::function<R(T, Rest...)>& f) :
		_combined(f)
	{}
	std::function<R(T, Rest...)> _combined;

	Parser<R> parse(const Parser<T>& pt, )

}
#endif

template<typename R, typename T>
struct Combinator
{
	Combinator(const std::function<R(T)>& f) :
		_combined(f)
	{}
	std::function<R(T)> _combined;

	Parser<R> operator()(const Parser<T>& pt) const
	{
		return pt >>= [this] (const T& t) {
			return_(_combined(t));
		};
	}
};

template<typename PT>
struct do_
{
	template<
		typename Func,
		typename RT = typename std::result_of<PT&(String)>::type,
		typename T  = typename RT::type,
		typename S  = typename std::result_of<Func&(T)>::type
	>
	static Combinator<S, T> yield(const Func& f)
	{
		return Combinator<S, T>(f);
	}

};


}
/* namespace Expression */
