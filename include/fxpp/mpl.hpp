/*
  Meta-programming library

  20190126
  Vishal Sood
*/

namespace fxpp { namespace meta {

template<
  template<typename...> class L, //some kind of list
  typename Head,
  typename... Tail>
struct cons{
  using type = L<Head, typename cons<L, Tail...>::type>;};

template<
  template<typename...> class L,
  typename Head>
struct cons<L, Head> {
  using type = L<Head>;};

} /*namespace meta*/ } /*namespace fxpp*/
