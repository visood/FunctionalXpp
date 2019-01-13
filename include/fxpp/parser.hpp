/*
  Functional parsers.
  Based on Graham Hutton's book on Haskell
 */

#pragma once
#include <regex>
#include <algorithm>
#include <stdexcept>
#include <clocale>
#include "fxpp/wrapped.hpp"
#include "fxpp/monadic.hpp"

//macros for nicer parser syntax
#define return_(x) return yield(x)

using namespace std::wrapped;

namespace expression
{
template<typename T>
struct ParsedResult {
	using type = T;
	ParsedResult(
		const T t,
		const String& s
	): value(t),
	   out(s),
	   empty(false)
	{}
	ParsedResult(
	): value(T()),
	   out(""),
	   empty(true)
	{}

	T value;
	String out;
	bool empty;
};
//template specialization when there is no value!
template<>
struct ParsedResult<void>{
	using type = void;
	ParsedResult(
		const String& s
	): out(s),
	   empty(false)
	{}
    ParsedResult(
    ): out(""),
       empty(true)
    {}
    String out;
    bool empty;
};
/*
  When parsing succeeds,
  we return 'some'
 */
template<typename T>
ParsedResult<T> some(
    const T& t,
    const String& s
){return
    ParsedResult<T>(t, s); }

ParsedResult<void> some(
    const String& s
){return
    ParsedResult<void>(s);}

template<typename T>
const ParsedResult<T>
empty = ParsedResult<T>();

template<>
const ParsedResult<void>
empty<void> = ParsedResult<void>();

template<typename T>
using Parser = std::function<ParsedResult<T>(String)>;

//I will use a utility function to parse
template<typename T>
inline ParsedResult<T> parse(
    const Parser<T>& pt,
    const String& in
){return pt(in);}
//that I may use with auto!
inline auto parse(
    const auto& pt,
    const String& in
){return pt(in);}

//a parser that always fails
template<typename T>
const Parser<T> failure = Parser<T>(
    [=] (const String& in){
        in + "suppress unused variable warning";
        return empty<T>;});

/*
  Parser is a monad, and to provide full monadic functionality,
  we define a monadic return
 */
template<typename T>
inline const Parser<T> yield(
    const T& t
){return
    Parser<T>(
        [=] (const String& in) -> ParsedResult<T>
            {return
                ParsedResult<T>(t, in);});}
/*
  may need a default constructor
 */
//template<>
const Parser<void> yield(
){return
    Parser<void>(
        [=] (const String& in) -> ParsedResult<void>
        {return
            some(in);});}
#if 0
template<typename T>
inline const auto yield(
    const T& t
){return
    [=] (const String& in)
    {return
        some(t, in);};}

inline const auto yield =
[] (const auto x)
{return
    [=] (const String& in)
    {return
        some(x, in);};};
#endif

//an operator to bind
template<
    typename T,
    typename S>
inline Parser<S> operator >>= (
    const Parser<T>& pt,
    const std::function< Parser<S>(T) >& fst
){return
    Parser<S>(
        [=] (const String& in) {
            const auto rt = parse(pt, in);
            if (rt.empty) return empty<S>;
            return parse(
                fst(rt.value),
                rt.out); });}
template<
    typename S>
inline Parser<S> operator >>= (
    const Parser<void>& p,
    const std::function< Parser<S>() >& fs
){return
    Parser<S> (
        [=] (const String& in) {
            const auto rt = parse(pt, in);
            if (rt.empty) return empty<S>;
            return parse(fs(), rt.out); });}

/*
  We will also bind using lambdas
 */
template<
    typename T,
    typename Function, //that returns a Parser<S>
    typename PS = typename std::result_of<Function&(T)>::type,
    typename RS = typename std::result_of<PS&(String)>::type,
    typename S  = typename RS::type>
inline Parser<S> operator >>= (
    const Parser<T>& pt,
    const Function& fst
){return
    Parser<S> (
        [=] (const String& in) {
            const auto rt = parse(pt, in);
            if (rt.empty) return empty<S>;
            return parse(fst(rt.value), rt.out); });}
template<
    typename Function, //that returns a Parser<S>
    typename PS = typename std::result_of<Function&()>::type,
    typename RS = typename std::result_of<PS&(String)>::type,
    typename S  = typename RS::type>
inline Parser<S> operator >>= (
    const Parser<void>& pt,
    const Function& fst
){return
    Parser<S> (
        [=] (const String& in) {
            const auto rt = parse(pt, in);
            if (rt.empty) return empty<S>;
            return parse(fst(), rt.out); });}
/*
  What if the Parser is also a lambda?
  More type deduction!
*/
template<
    typename PT, //a type that quacks like a Parser<T>
    typename Function,
    typename RT = typename std::result_of< PT&(String) >::type,
    typename T  = typename RT::type,
    typename PS = typename std::result_of< Function&(T) >::type,
    typename RS = typename std::result_of< PS&(String)>::type,
    typename S  = typename RS::type>
inline const Parser<S> operator >>= (
    const PT& pt,
    const Function& fst
){return
    [=] (const String& in) {
        const auto rt = pt(in);
        if (rt.empty) return empty<S>;
        return fst(rt.value)(rt.out); };}
template<
    typename P, //a type that quacks like Parser<void>
    typename Function,
    typename PS = typename std::result_of<F&()>::type,
    typename RS = typename std::result_of<PS&(String)>::type,
    typename S  = typename RS::type>
inline const Parser<S> operator >>= (
    const P& p,
    const Function& fs
){return
    [=] (const String& in) {
        const auto r = p(in);
        if (rt.empty) return empty<S>;
        return fst()(rt.out);};}
/*
  What if do not use the return of the first parser?
  I will define this operation as >>
 */
template<
    typename S>
inline Parser<S> operator >> (
    const auto& pt,
    const Parser<S>& ps
){return
    Parser<S>(
        [=] (const String& in) {
            const auto rt = parse(pt, in);
            if (rt.empty) return empty<S>;
            return parse(ps, rt.out); });}

        
}
#undef return_
