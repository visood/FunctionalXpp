/*
  Meta-programming library

  20190126
  Vishal Sood
*/

namespace fxpp { namespace meta {

template<
  template<typename...> class Container, //some kind of list
  typename Head,
  typename... Tail>
struct cons{
  using type = Container<Head, typename cons<Container, Tail...>::type>;};

template<
  template<typename...> class Container,
  typename Head>
struct cons<Container, Head> {
  using type = Container<Head>;};

} /*namespace meta*/ } /*namespace fxpp*/
