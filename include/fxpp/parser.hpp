/*
  Functional parsers.
  Based on Graham Hutton's book on Haskell
 */

#pragma once
#include <regex>
#include <algorithm>
#include <stdexcept>
#include <clocale>
#include "fxpp/fxpp.hpp"
#include "fxpp/monadic.hpp"
#include "fxpp/list.hpp"

//macros for nicer parser syntax
#define return_(x) return yield(x)

using namespace fxpp;
using namespace fxpp::collection;

#if 0
template<
  typename T>
inline fxpp::collection::List<T>& operator >> (
  const T& head,
  fxpp::collection::List<T>& tail
){tail.push_front(head);
  return tail;}
template<
  typename T>
inline const fxpp::collection::List<T>& operator >> (
  const T& head,
  fxpp::collection::List<T>& tail
){tail.push_front(head);
  return std::move(tail);}
#endif

namespace fxpp {
namespace expression {
template<typename T>
struct ParsedResult{
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
	bool empty; };
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
/*
  That we may use with auto!
  However clang says: 'auto' not allowed in function prototype
*/
#if 0
inline auto parse(
    const auto& pt,
    const String& in
){return pt(in);}
#endif
template<
  typename ParserType, //Parser<T>
  typename ResultType = typename std::result_of<ParserType&(String)>::type>
inline ResultType parse(
  const ParserType& p,
  const String& in
){return p(in);}


//a parser that always fails
template<typename T>
const Parser<T> failure=
  Parser<T>(
    [] (const String& in){
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
            const auto rt = parse(p, in);
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
    typename PS = typename std::result_of<Function&()>::type,
    typename RS = typename std::result_of<PS&(String)>::type,
    typename S  = typename RS::type>
inline const Parser<S> operator >>= (
    const P& p,
    const Function& fs
){return
    [=] (const String& in) {
        const auto r = p(in);
        if (r.empty) return empty<S>;
        return fs()(r.out);};}
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
/*
  To think of a parser as a monad we have to imagine it as a container, like a
  list. There is one essential difference thought. An empty list does not
  contain any elements at all. However our Parser when empty may contain an 
  'out' string.
  We can define a 'Nothing' subclassing Parser.
  Or we can mimic the effect of such a 'null' type through a bind function '>>='
  that takes a function with no arguments (which strictly speaking is not a
  function at all!). This function will implicitly convert any Parser on the
  left into a Parser without any value type (dropping parsed values). The
  resulting syntax when chaining Parsers will be cleaner and clearer (no need
  for '>>', '>>=' will be used everywhere). However a parser of null type can
  also be defined using a template specialization that takes no parameters ---
  but our Parser is not a class!
 */
#if 0
template<
    typename Function,
    typename PS = typename std::result_of<Function&()>::type,
    typename RS = typename std::result_of<PS&(String)>::type,
    typename S = typename RS::type>
inline Parser<S> operator >>= (
    const auto& p,
    const Function& f
){return p >> f();}
#endif

template<
    typename PS,
    typename RS = typename std::result_of<PS&(String)>::type,
    typename S  = typename RS::type>
inline Parser<S> operator >> (
    const auto& p,
    const auto& ps
){return
    Parser<S>(
        [=] (const String& in) {
            const auto r = parse(p, in);
            if (r.empty) return empty<S>;
            return parse(ps, r.out); });}

/*
  We may want the parser to produce a tuple, 
 */
template<
    typename T,
    typename S>
inline Parser< std::tuple<T, S> > operator & (
    const Parser<T>& pt,
    const Parser<S>& ps
){return
    pt >>= [ps] (const T& t
    ){return
		ps >>= [=] (const S& s
        ){return 
            yield(std::make_tuple(t, s));};};}
/*
  specialize to the case when the second Parser drops its result
*/
template<
    typename T>
inline Parser< std::tuple<T> > operator & (
    const Parser<T>& pt,
    const Parser<void>& p
){return
    pt >>= [p] (const T& t
    ){return
        p >> yield(std::make_tuple(t));};}
template<
	typename Next,
	typename... This>
inline auto operator &(
	const Parser< std::tuple<This...> >& pthis,
	const Parser<Next>& pnext
){return
	pthis >>= [=] (const std::tuple<This...>& ts
	){return
		pnext >>= [=] (const Next& n
		){return yield(
				std::tuple_cat(ts, std::make_tuple(n)));};};}
/*
  Specialization to when Parser<Next> is Parser<void>
 */
template<
	typename... This>
inline auto operator &(
	const Parser< std::tuple<This...> >& pt,
	const Parser<void>& p
){return
	pt >>= [p] (const std::tuple<This...>& ts
	){return
		p >> yield(ts);};}
/*
  Now we can define some 'atomic' parsers,
  that we can combine to build complex parsers.
*/
const auto item = Parser<char>(
	[] (const String& s){
		if (s.empty()) return empty<char>;
		return some(s[0], s.substr(1));});
/*
  We would like the next parser to be shorter,
  but function templates cannot be specialized
 */
template<
	typename T,
	size_t N>
struct RepeatImpl{
	auto from(const Parser<T>& pt
	){return
		pt >>= [=] (const auto& t
		){return
			RepeatImpl<T, N - 1>().from(pt) >>= [=] (const auto& ts
			){return
				yield(
					std::tuple_cat(
						std::make_tuple(t),
						ts));};};}};
template<
	typename T>
struct RepeatImpl<T, 1>{
	auto from(const Parser<T>& pt){return
		pt >>= [=] (const auto& t){return
			yield(
				std::make_tuple(t));};}};
template<
	size_t N,
	typename T>
inline const auto repeat(
	const Parser<T>& pt
){return
    RepeatImpl<T, N>().from(pt);}
/*
  If a parser fails, we may want to try another
 */
template<
	typename T>
inline Parser<T> operator |(
	const Parser<T>& pt1,
	const Parser<T>& pt2
){return
	Parser<T>(
		[=](const String& in){
			const auto rt1 = parse(pt1, in);
			if (rt1.empty) return parse(pt2, in);
			return rt1;});}
/*
  If a parser failes, we may want to move on to the next
 */
template<
	typename T>
inline Parser<T> maybe(
	const Parser<T>& pt
){return
	pt | yield(T());}
inline Parser<void> maybe(
	const Parser<void>& p
){return
	p | yield();}

/*
  Here is another way to drop the result of a parser
*/
template<
	typename T>
inline Parser<void> drop(
	const Parser<T>& pt
){return
	pt >> yield();}
/*
  There may be many occurrences of a pattern. We define the parser 'many' below.
  'many' will be slow if it has to parse a large number of items.
  Passing non-const reference causes incomprehensible compile errors
  We will also provide an imperative several to perform the same operation.
 */
template<
	typename T>
inline Parser< List<T> > many(
	const Parser<T>& pt
){return
    many1(pt) | yield(List<T>());}

using ::operator >>;
template<
	typename T>
inline Parser< List<T> > many1(
	const Parser<T>& pt
){return
    pt >>= [=] (const T& t){
      return
        many(pt) >>= [=] (const List<T>& ts){
          return
            yield(t >> ts);};}; }
#if 0
//imperative many
template<
	typename T>
inline Parser< List<T> > several(
	const Parser<T>& pt
){return
	Parser< List<T> >(
		[=] (const String& in){
			List<T> lt;
			String finalOut = in;
			auto rt = parse(pt, in);
			while (not rt.empty) {
				lt.push_back(rt.value);
				finalOut = rt.out;
				rt = parse(pt, rt.out);}
			return some(lt, finalOut);});}
#endif
/*
  Or we can use a generalization of 'several'
*/
template<
    typename T, //type of the parser
    typename L, //type to accunmulate in
    typename F> //function type to accumulate with
inline Parser<L> accumulate(
    const Parser<T>& pt,
    const L& l,
    const F& f
){return
    Parser<L> (
        [=] (const String& in) {
            String finalOut = in;
            auto rt = parse(pt, in);
            L lmr = l;
            while (not rt.empty) {
                lmr = f(lmr, rt.value);
                finalOut = rt.out;
                rt = parse(pt, rt.out);}
            return some(lmr, finalOut);});}
/*
  We may accumulate into a trivial value of L
 */
template<
    typename L,
    typename T,
    typename F>
inline Parser<L> accumulate(
    const Parser<T>& pt,
    const F& f
){return
    accumulate(pt, L(), f);}
template<
    typename T>
inline Parser< List<T> > several(
    const Parser<T>& pt
){return
    accumulate< List<T> >(
        pt,
        [=] (List<T>& l, const T& t) {
            l.push_back(t);
            return l;});}
template<
    typename T>
inline Parser<int> freq(const Parser<T>& pt
){return
    accumulate(
        pt >> yield(1U),
        0U,
        [=] (const int s, const int t) -> int {return
            s + t;});}
/*
  A parser that succeeds given the success of a predicate
 */
inline Parser<char> sat(
    const auto& pred
){return
    item >>= [=] (const char c){
      return
        pred(c) ? yield(c) : failure<char>;};}

const auto isSpace=
  [=] (const char c){
    return c == ' ';};
inline const auto char_(
    const char c
){return
    sat([=] (const char x){
      return
        x == c;});}
inline Parser<void> drop(
    const char c
){return
    char_(c) >> yield();}

inline Parser<void> drop(
    const String& s
){return 
    s.size() == 0 ?
    yield() : char_(s[0]) >> drop(s.substr(1));}
    
const auto space       = many(sat(isSpace)) >> yield();
const auto nline       = char_('\n');
const auto tab         = char_('\t');
const auto whitespace  = char_(' ') | nline | tab;
const auto period      = char_('.');

/*
  remove space
*/
template<
    typename T>
inline Parser<T> token(
    const Parser<T>& pt
){return space >>= [=] (){
    return pt >>= [=] (const T& t){
        return
          space >>= [=] (){
              return
                yield(t);};};};}

/*
  regex enabled parsers
*/
inline Parser<String> capture(
    const String& pattern
){return
    Parser<String>(
      [=] (const String& in) {
        std::smatch matches;
        std::regex mexp(
          String("(") + pattern + ")(.*)");
        if (std::regex_match(in, matches, mexp))
        return some(
          matches[1].str(),
          matches[2].str());
        return empty<String>; });}
}
}

#undef return_
