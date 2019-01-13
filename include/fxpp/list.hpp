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

template<typename T>
using List = std::list<T>

template<typename T>
inline const T& head(const List<T>& list) {return *begin(list); }

template<typename T>
inline List<T> tail(
	const List<T>& list
){return List<T>(++begin(list), end(list)); }

template<typename T>
inline List<T> flatten(
    List< List<T> >& xss
){List<T> ys;
  auto it = ys.begin();
  for (const auto& xs: xss) ys.insert(it, xs); 
  return ys; }

template<
	typename T,
	typename F,
	typename S = typename std::result_of<F&(T)>::type>
inline List<S> map(
    const F f,
    const List<T>& ts
){List<S> ss;
  for (const auto& t : ts) ss.push_back(f(t));
  return ts; }

template<typename T>
List<T> nil = List<T>();

}/*namespace collection */
}/*namespace fxpp */

/*
  To mimic a cons list, we use the stream operator
 */

template<typename T>
inline fxpp::collection::List<T>& operator >> (
    const T& head,
    fxpp::collection::List<T>& tail
){tail.push_front(head);
    return std::move(tail);}

//bind
template<
    typename T,
    typename F,
    typename S = typename std::result_of<F&(T)>::value_type>
inline fxpp::collection::List<S>& operator >>= (
    fxpp::collection::List<T>& ts,
    const F& fst
){ return fxpp::collection::flatten( fxpp::collection::map(fs, ts)); }

template<typename T>
inline bool operator ==(
    const fxpp::collection::List<T>& l1,
    const fxpp::collection::List<T>& l2
){if (l1.empty()) return l2.empty();
    if (l2.empty()) return false;
    return (
        fxpp::collection::head(l1) == fxpp::collection::head(l2) and
        fxpp::collection::tail(l1) == fxpp::collection::tail(l2)); }
