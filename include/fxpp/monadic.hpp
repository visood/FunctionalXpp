/*
  Monadic operations on containers...
  I will keep adding my further monadic explorations here.
*/

#include "fxpp/view.hpp"

using namespace fxpp;

namespace fxpp { namespace monadic {
template<
    template<typename...> class C, //container type
    typename E, //element type
    typename M, //type of the mapping function
    typename R = typename std::result_of<M&(E)>::type>

C<R> operator >= (const C<E>& xs, const M& func){return
    fxpp::view(xs).map(func).collect(); }

template<
    template<typename...> class C, //container type
    typename E, //element type 
    typename P> //predicate type
C<E> operator &= (const C<E>& xs,
                  const P& predicate){return
    fxpp::view(xs).filter(predicate).collect(); }
}}
