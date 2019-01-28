/*
  A sort of static JSON
  Vishal Sood
  20190124
*/
#include "fxpp/mpl.hpp"
#include "fxpp/maybe.hpp"

namespace fxpp { namespace property { 

template<typename... Args>
struct PTree;

template<typename Element>
struct PLeaf{
  using type= PLeaf<Element>;
  using value_type= Element;
  static constexpr char const* role = "leaf";
  const char* key; //how can String be static!
  Element value; };

template<typename Element>
constexpr char const* PLeaf<Element>::role;

template<typename PType>
struct PNode {
  using type = PNode<PType>;
  using value_type = PType;
  static constexpr char const* role = "node";
  const char* key;
  PType value;};

template<>
struct PTree<> {
  using type = PTree<>;
  using value_type = void();
  static constexpr char const* key = "";
  using valued_ptree_type = PTree<>;};

constexpr PTree<> PNull;

template<typename PType>
struct PTree< PNode<PType> > {
  using type = PTree< PNode<PType> >;
  using head_type = PNode<PType>;
  PNode<PType> head;};

template<typename Element>
struct PTree< PLeaf<Element> > {
  using type = PTree< PLeaf<Element> >;
  using head_type = PLeaf<Element>;
  PLeaf<Element> head;};

template<
  typename Element,
  typename... Args>
struct PTree< PLeaf<Element>, PTree<Args...> > {
  using type = PTree< PLeaf<Element>, PTree<Args...> >;
  using head_type = PLeaf<Element>;
  PLeaf<Element> head;
  PTree<Args...> tail;};

template<
  typename PType,
  typename... Args>
struct PTree< PNode<PType>, PTree<Args...> > {
  using type = PTree< PNode<PType>, PTree<Args...> >;
  using head_type = PNode<PType>;
  PNode<PType> head;
  PTree<Args...> tail;};

template<
  typename Head,
  typename... Tail>
using ptree_cons = meta::cons<PTree, Head, Tail...>;

template<
  typename Element>
Maybe::Maybe<Element> get_value(
  const PLeaf<Element>& leaf,
  const char* key)
{
  if (leaf.key != key)
    return Maybe::None<Element>();
  return leaf.value;
}

// template<
//   typename Element,
//   typename... Args>
// Element get_value(
//   const PTree< PLeaf<Element>, PTree<Args..> >& ptree,
//   const char* key);

// template<
//   typename PType,
//   typename... Args>
// Element get_value(
//   const PTree< PNode<PType>, PTree<Args...> >& ptree,
//   const char* key);
} /*namespace property */
} /*namespace fxpp */
