#include "utils.h"

template <typename List, typename T >
using liter = List<T>::iterator

template <typename List, typename T >
using lends = std::pair< liter, liter >

template <typename List, typename T, typename Func >
std::pair < List<T>, List<T> >
split_if(List<T>& xs, const Func& is_split_point) {
  auto spoint = std::find(xs.begin(), xs.end(), is_split_point);
  if (spoint != end) {
    List<T> first(spoint + 1 - begin());
    List<T> second(end - spoint - 1 );
    return std::make_pair(first, second);
  } else {
    return std::make_pair(xs, List<T>());
  }
}

template < typename List, typename T, typename BFunc, typename Sfunc>
liter<List, T> squash_if(List<T>& xs,
                      const uint delta,
                      const Bfunc do_squash,
                      const SFunc& squash,
                      const split = zero_value<T>()) {
  auto delta_each_side = [xs&] (const uint i) {
    return elements_around(xs, i, delta);
  }
  auto store_at = xs.first;
  for(auto itr = xs.first; itr != xs.second; ++itr; ++store_at) {
    if (do_squash(*itr)) {
      Maybe<T> s = squash(delta_each_side(itr, split));
      if (s.isValid()) {
        *store_at = s.get();
        --store_at;
      }
    }
  }
  return store_at;
}

//example squashers
template < typename List, typename T >
Maybe<T> added(const List<T>& xs, const T split = zero_value<T>()) {
  if (xs.begin() == xs.end()) return None();
  const T s = std::accumulate(std::begin(xs), std::end(xs) - 1, zero_value<T>(),
                              [] (const T& s, const T& x) {return s + x + split;});
  return Valid( s + *(std::end(xs) - 1) );
}

template < typename List, typename T >
Maybe<T> secondHalf(const List<T>& xs, const T split = zero_value<T>()) {
  uint n = (uint) std::size(xs);
  const List<T> sh(std::begin(xs) + n/2, std::end(xs));
  return added(sh, split);
}
