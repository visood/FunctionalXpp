/*
  Functional List.
  I have started with a simple wrapper around std::list,
  that I hope to replace with an implementation of a truly
  functional (and persistent) list.
  It should be doable in C++, however I have not found any implementation.

  Vishal Sood
  20190113
 */
#pragma once

#include <list>
namespace fxpp {namespace collection {

//a const list can be a good example to design an immutable,
//persistent data structure in C++.
// a wrapper around std::list that employs monadic relationships
template <typename T>
using List = std::list<T>;
//we do not want to create a wrapper class, instead we use operators
template<typename T>
List<T> unit(const T& t) { return List<T>(t);}

template<typename T>
inline const T& head(const List<T>& list) { return *begin(list);}

template<typename T>
inline List<T> tail(const List<T>& list) {
	return List<T>(++begin(list), end(list));
}



//for a list monad we need to flatten a list of lists
template<typename T>
inline List<T> flatten(List< List<T> >& llt)
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
template<typename T>
List<T> nil = List<T>();
}/*namespace collection */
}/*namespace fxpp */


//the list should be cons list, we will use the stream operator
//this is not the ideal solution
//ideally we would like >> to create a new (persistent, immutable) list
//checkout implementation attempt in list.h
template<typename T>
inline fxpp::collection::List<T>& operator >> (
  const T& head,
  fxpp::collection::List<T>& tail)
{
	tail.push_front(head);
	return tail;
}

template<typename T>
inline const fxpp::collection::List<T>&
operator >> (
  const T& head,
  fxpp::collection::List<T> tail)
{
	tail.push_front(head);
	return std::move(tail);
}
//bind
template<
	typename T,
	typename F,
	typename S = typename std::result_of<F&(T)>::value_type
>
inline fxpp::collection::List<S>&
operator >>= (
  fxpp::collection::List<T>& ts,
  const F& fst)
{
	return flatten( map(fst, ts));
}
template<typename T>
inline bool operator==(
  const fxpp::collection::List<T>& l1,
  const fxpp::collection::List<T>& l2)
{
	if (l1.empty()) return l2.empty();
	if (l2.empty()) return false;
	return
    fxpp::collection::head(l1) == fxpp::collection::head(l2) and
    fxpp::collection::tail(l1) == fxpp::collection::tail(l2);
}





