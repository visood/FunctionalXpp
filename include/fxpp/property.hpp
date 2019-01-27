/*
  A sort of static JSON
  Vishal Sood
  20190124
*/
#include "fxpp/mpl.hpp"

namespace fxpp { namespace property { 

template<typename Element>
struct PLeaf{
  using type= PLeaf<Element>;
  using value_type= Element;
  static constexpr char const* role = "leaf";
  const String key; //how can String be static!
  Element value; };

template<typename PType>
struct PNode {
  using type = PNode<PType>;
  using value_type = PTree;
  static constexpr char const* role = "node";
  const String key;
  PType value;};


template<typename... Args>
struct PTree;

template<>
struct PTree<> {
  using type = PTree<>;
  using valued_ptree_type = PTree<>;};

constexpr Ptree<> PNull;

template<typename PType>
struct PTree< PNode<PType> > {
  using type = PTree< PNode<PType> >;
  using head_type = PNode<PType>;
  using value_ptree_type = PTree< PNode<PType> >;
  PNode<PType> head;};

template<typename Element>
struct PTree< PLeaf<Element> > {
  using type = PTree< PLeaf<Element> >;
  using head_type = PLeaf<Element>;
  using valued_ptree_type = PTree< PLeaf<Element> >;
  PLeaf<Element> head;};

template<
  typename Element,
  typename... Args>
struct PTree< PLeaf<Element>, PTree<Args...> > {
  using type = PTree< PLeaf<Element>, PTree<Args...> >;
  using head_type = PLeaf<Element>;
  using valued_ptree_type = type;
  PLeaf<Element> head;
  PTree<Args...> tail;};

template<
  typename PType,
  typename... Args>
struct PTree< PNode<PType>, PTree<Args...> > {
  using type = PTree< PNode<PType>, PTree<Args...> >;
  using head_type = PNode<PType>;
  using valued_ptree_type = type;
  PNode<PType> head;
  PTree<Args...> tail;};

template<
  typename Head,
  typename... Tail>
using ptree_cons = meta::cons<PTree, Head, Tail...>;

/*
  blank properties, i.e. properties without values
  these properties can represent the nested type of a tree property with values
*/
template<typename Element>
struct __PLeaf {
  using value_type = Element;
  static constexpr char char* role = "leaf";
  const String key;};};

template<typename PType>
struct __PNode{
  using value_type = PType;
  static constexpr char const* role = "node";
  const String key;
  PType value;};

template<typename... Args> struct __PTree;

template<>
struct __PTree<> {
  using value_type = void;
  using valued_ptree_type = PTree<>;};

template<typename Element>
struct __PTree<__PLeaf<Element> >{
  using head_type = __PLeaf<Element>;
  using valued_ptree_type = PTree< PLeaf<Element> >;
  __PLeaf<Element> head;};

template<typename PType>
struct __PTree< __PNode<PType> > {
  using head_type= __PNode<PType>;
  using valued_ptree_type=
    PTree< PNode<typename PType::valued_ptree_type> >;
  __PNode<PType> head;};

template<
  typename Element,
  typename... Args>
struct __PTree< __PLeaf<Element>, __PTree<Args...> > {
  using type = __PTree< __PLeaf<Element>, __PTree<Args...> >;
  using head_type = __PNode<Element>;
  using valued_ptree_type= PTree<
    PLeaf<Element>,
    typename __PTree<Args...>::valued_ptree_type>;
  __PLeaf<Element> head;
  __PTree<Args...> tail;};

template<
  typename PType,
  typename... Args>
struct __PTree< __PNode<PType>, __PTree<Args...> >{
  using type = __PTree< __PNode<PType>, __PTree<Args..> >;
  using head_type = __PNode<PType>;
  using valued_ptree_type= PTree<
    PNode< typename PType::valued_ptree_type >,
    typename __PTree<Args...>::valued_ptree_type >;
  __PNode<PType> head;
  __PTree<Args...> tail; };

template<
  typename Head,
  typename... Tail>
using __ptree_cons = meta::cons<__PTree, Head, Tail...>
    
}}
