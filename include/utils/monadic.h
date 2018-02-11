#include "util.h"
#include <list>
//for the parser to be a monad we follow haskell

//a monad of type T requires a 'return', which we implement
//as a function yield, outside any class

namespace Monadic {
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
T& head(const List<T>& list) { return *begin(list);}

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



#if 0
//Parser as a class, dropped in favor of std::function / lambda implementations
//from a parser, we need to return the result of its action on string
template< typename T >
class Parser
{
public:

	template <typename S>
	Parser<S> flatMap( std::function<Parser<S>(T)> fpst) const
	{
		return Parser<S> (
			[=] (const String& in) -> ParsedResult<S> {
				const auto rt = parse(_apply, in);
				if (rt.empty) return nothing<T>;
				return parse(fpst(rt.value), rt.out);
			}
		);
	}

	template <typename S>
	Parser<S> map( std::function<S(T)> fst) const
	{
		return Parser<S> (
			[=] (const String& in) -> ParsedResult<S> {
				const auto rt = parse(_apply, in);
				if (rt.empty) return nothing<T>;
				return result(fst(rt.value), rt.out);
			}
		);
	}

	//seemingly map can be written using a flatMap!
	template <typename S>
	Parser<S> mapy( std::function<S(T)> fst) const
	{
		return flatMap( [=] (const T t) {
				yield (t);
			}
		);
	}


private:
	std::function<ParsedResult<T>(String)> _apply;
};

//monadic return must sit outside the class as a function
template<typename T>
Parser<T> unit(const T& t)
{
  return Parser<T>(
    [=] (const String& in) {return some(t, in);}
  );
}


//a helper function to construct Parser
template <
	typename Function,
	typename T = typename std::result_of<Function&(String)>::type::data_type
>
Parser<T> parser(const Function f)
{
	return Parser<T> (f);
}
//an example, assuming word has been defined
using TwoWordTup = std::tuple<Word, Word>;
const auto twoWords = word.flatMap([=] (const Word& w1) {
		return word.map([=] (const Word& w2) {
				return std::make_tuple(w1, w2);
			}
		);
	}
);
//replacing map with flatMap and yield
const auto twoWords = word.flatMap( [=] (const Word& w1) {
		return word.flatMap( [=] (const Word& w2) {
				return yield(std::make_tuple(w1, w2));
			}
		);
	}
);

//using operators, haskell style
const auto twoWords = word >>= [=] (const Word& w1) {
	return word >>= [=] (const Word& w2) {
		return yield(std::make_tuple(w1, w2));
	};
};

const auto threeWords = word.flatMap ( [=] (const Word& w1) {
		return word.flatMap ( [=] (const Word& w2) {
				return word.map ([=] (const Word& w3) {
						return std::make_tuple(w1, w2, w3);
					}
				);
			}
		);
	}
);

//a generic repeat
template<typename T>
auto three(const Parser<T>& pt)
{
	return pt >>= [=] (const T& t1) {
		return pt >>= [=] (const T& t2) {
			return pt >>= [=] (const T& t3) {
				return yield(std::make_tuple(t1, t2, t3));
			}
		};
	};
}
#endif


} /* namespace Monadic */
