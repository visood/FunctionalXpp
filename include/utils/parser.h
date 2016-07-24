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

template <typename T, typename S>
Parser<S> operator >>= (
	const Parser<T> pt,
	const std::function<Parser<S>(T)>& fps
) {
	return Parser<S> (
		[=] (const String& in) -> ParsedResult<S> {
			ParsedResult<T> rt = parse(pt, in);
			if (rt.empty) return ParsedResult<S>();
			return parse(fps(rt.value), rt.out);
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

const auto space = [](const char c)->bool {
	return c == ' ';
};
const auto nline = [](const char c)->bool {
	return c == '\n';
};
const auto whitespace = [](const char c) -> bool {
	return space(c) or nline(c);
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

#if 0
template<typename T>
struct FallBack : public Parser<T>
{
	FallBack(const Parser<T>& pt) : Parser<T>(pt) {}
};

template<typename T>
Parser<T> operator & (
	const Parser<T>& pt,
	const FallBack<T>& ps
){
	return Parser<T> (
		[=] (const String& in) -> ParsedResult<T> {
			const auto r = parse(pt, in);
			if (r.empty) return parse(ps, in);
			return r;
		}
	);
}

template<typename T>
Parser<T> orElse(const Parser<T>& pt)
{
	return FallBack<T>(pt);
}

#endif

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

Parser<String> word(
	item >>= bind<char> (
		[=] (const char c) -> Parser<String> {
			if (c == ' ' or c == '\n') return yield(String());
			return yield(String() + c) + word;
		}
	)
);



} /* namespace Expression */
