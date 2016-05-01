#include "utils.h"
#include "kmer.h"

template < template <typename S> typename C<S>, typename T, typename Func >
std::pair< C<T>, C<T> >
split_at (C<T>& xs, const Func& split_point) {
  auto spoint = std::find(std::begin(xs), std::end(xs), split_point);
  return std::make_pair(C<T>(std::begin(xs), spoint), C<T>(spoint, std::end(xs)))
}

template < template <typename S> C<S>, typename T>
C<T> elements_upto(const C<T>& xs, const uint i, const uint n, const T insert_default) {
  C<T> ys(n);
  auto copy_begin = std::begin(xs) + (i - n > 0 ? i - n :   0  );
  auto fill_end  =  std::begin(ys) + (i - n > 0 ?   0   : n - i);
  std::copy(copy_begin, std::begin(xs) + i, fill_end);
  std::fill(std::begin(y), fill_end, insert_default);
  return ys;
}

template < template <typename S > typename C, typename T>
C<T> elements_after(const C<T>& xs, const uint i, const uint n, const T insert_default) {
  C<T> ys(n);
  uint l = (uint) xs.size();
  auto copy_end = (i + 1 + n < l) ? std::begin(xs) + i + 1 + n : std::end(xs);
  auto fill_begin = (i + 1 + n < l) ? std::end(ys) : std::begin(ys) - l + i + 1;
  std::copy(std::begin() + i + 1, copy_end, std::begin(ys));
  std::fill(fill_begin, std::end(ys), insert_default);
  return ys;
}


template <template <typename S> typename C<S>, typename T>
C<T> elements_around(const C<T>& xs, const uint i, const uint n, const T insert_default ) {
  C<T> ys(2*n + 1);
  auto upto = elements_upto(xs, i, n, insert_default);
  auto after = elements_after(xs, i, n, insert_default);
  auto cend = copy(std::begin(upto), std::end(upto), std::begin(ys));
  *cend = xs[i];
  copy(std::begin(after), std::end(after), cend + 1);
  return ys;
}
