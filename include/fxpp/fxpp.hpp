/*
  functional utilities
*/

#include <iostream>
#include <string>
#include <type_traits>
#include <memory>
#include <functional>
#include <numeric>
#include <algorithm>
#include <set>
#include <vector>
#include <exception>

namespace fxpp {
using String= std::string;

template<
  template<typename...> class Container,
  typename Element>
void puts(
  const Container<Element>& elements) 
{ std::for_each(
    begin(
      elements),
    end(
      elements),
    [](const Element& x){
      std::cout << x << ", ";});
  std::cout << std::endl;}

template<
  template<typename...> class Container,
  typename T1,
  typename T2,
  typename R = std::pair<T1, T2>>
Container<R> zip(
  const Container<T1>& xs,
  const Container<T2>& ys
){const auto l = xs.size() <= ys.size() ? xs.size() : ys.size();
  Container<R> xys(l);
  std::transform(
    begin(xs), begin(xs) + l,
    begin(ys),
    begin(xys),
    [] (const T1& x, const T2& y) -> R {
      return {x, y};});}

template<
  template<typename...> class Container,
  typename Element,
  typename Predicate>
bool any(
  const Predicate& predicate,
  const Container<Element>& elements
){return
    std::any_of(
      begin(
        elements), end(
          elements),
      predicate);}

template<
  template<typename...> class Container,
  typename Element,
  typename Predicate>
bool all(
  const Predicate& predicate,
  const Container<Element>& elements
){return
    std::all_of(
      begin(
        elements),
      end(
        elements),
      predicate);}

template<
  template<typename...> class Container,
  typename Element>
bool operator ==(
  const Container<Element>& xs,
  const Container<Element>& ys
){return
    xs.size() == ys.size()
    and all(
      [] (const auto& xy) {return xy.first == xy.second;},
      zip(xs, ys));}

template<
  template<typename...> class Container,
  typename Element,
  typename Comparator>
Element max(
  const Comparator& compare,
  const Container<Element>& elements)
{ if (elements.size() == 0)
    throw
      std::out_of_range(
        "max of an empty container");
  return
    *std::max_element(
      begin(
        elements),
      end(
        elements),
      compare);}

template<
  template<typename...> class Container,
  typename Element>
Element max(
  const Container<Element>& elements)
{ if (elements.size() == 0)
    throw
      std::out_of_range(
        "max of an empty container");
  return
    *std::max_element(
      begin(
        elements),
      end(
          elements));}

template<
  template<typename...> class Container,
  typename Element,
  typename Comparator>
Element maxOrElse(
  const Comparator& compare,
  const Container<Element>& elements,
  const Element& default_value
){return
    elements.size() == 0 ?
    default_value
    : *std::max_element(
      begin(
        elements),
      end(
        elements),
      compare);}

template<
  template<typename...> class Container,
  typename Element>
Element maxOrElse(
  const Container<Element>& elements,
  const Element& default_value
){return
    elements.size() == 0 ?
    default_value
    : *std::max_element(
      begin(
        elements),
      end(
        elements));}

 template<
  template<typename...> class Container,
  typename Element,
  typename Comparator>
Element min(
  const Comparator& compare,
  const Container<Element>& elements)
{ if (elements.size() == 0)
    throw
      std::out_of_range(
        "min of an empty container");
  return
    *std::min_element(
      begin(
        elements),
      end(
        elements),
      compare);}

template<
  template<typename...> class Container,
  typename Element>
Element min(
  const Container<Element>& elements)
{ if (elements.size() == 0)
    throw
      std::out_of_range(
        "min of an empty container");
  return
    *std::min_element(
      begin(
        elements),
      end(
        elements));}

template<
  template<typename...> class Container,
  typename Element,
  typename Comparator>
Element minOrElse(
  const Comparator& compare,
  const Container<Element>& elements,
  const Element& default_value
){return
    elements.size() == 0 ?
    default_value :
    *std::min_element(
      begin(
        elements),
      end(
        elements),
      compare);}

template<
  template<typename...> class Container,
  typename Element>
Element minOrElse(
  const Container<Element>& elements,
  const Element& default_value
){return
    elements.size() == 0 ?
    default_value :
    *std::min_element(
      begin(
        elements),
      end(
        elements));}

template<
  typename Element,
  typename... Args>
Element max(
  const std::set<Element, Args...>& elements)
{ if (elements.empty())
    throw
      std::out_of_range(
        "max of an empty set");
  return
    *elements.rbegin();}

template<
  typename Element,
  typename... Args>
Element min(
  const std::set<Element, Args...>& elements)
{ if (elements.empty())
    throw std::out_of_range(
      "max of an empty set");
  return *elements.begin();}

template<
  typename Element,
  typename... Args>
Element maxOrElse(
  const std::set<Element, Args...>& elements,
  const Element& default_value)
{ return
    elements.empty() ?
    default_value :
    *elements.rbegin();}

template<
    typename Element,
    typename... Args>
    Element minOrElse(
      const std::set<Element, Args...>& elements,
      const Element& default_value)
  { return
      elements.empty() ?
      default_value :
      *elements.begin();}

template<
  template<typename...> class  Container,
  typename Element>
bool contains(
  const Element& y,
  const Container<Element>& elements)
{ return
    any(
      [&y](const Element& x){return x==y;},
      elements);}

template<
  typename T>
bool contains(
  const T& y,
  const std::set<T>& elements)
{ return
    elements.find(y) != elements.end();}

inline bool contains(
  const char c,
  const String& s)
{return
    s.find(c) != std::string::npos;}

template<
  template<typename...> class Container,
  typename Element>
Container<Element> intersection(
  const std::set<Element>& xs,
  const Container<Element>& ys)
{ Container<Element> zs;
  std::set_intersection(
    begin(xs), end(xs),
    begin(ys), end(ys),
    std::back_inserter(zs));
  return zs;}

template<
  template<typename...> class Container>
Container<String> split(
  const String& text,
  const char separator)
{ Container<String> tokens;
  String token;
  std::istringstream token_stream(text);
  while (std::getline(token_stream, token, separator))
    tokens.push_back(token);
  return tokens;}

template<
  template<typename...> class Container,
  typename ElementIn,
  typename Function,
  typename ElementOut = typename std::result_of<Function&(ElementIn)>::type >
Container<ElementOut> map(
  const Function& function,
  const Container<ElementIn>& elements)
{ Container<ElementOut>
    transformed_elements(
      elements.size());
  std::transform(
    begin(
      elements),
    end(
      elements),
    begin(
      transformed_elements),
    function);
  return
    transformed_elements;}


template<
  typename ElementIn,
  typename Function,
  typename ElementOut = typename std::result_of<Function&(ElementIn)>::type>
std::set<ElementOut> map(
  const Function& function,
  const std::set<ElementIn>& elements)
{ std::set<ElementOut> transformed_elements;
  for (const ElementIn& element: elements)
    transformed_elements.insert(
      function(
        element));
  return
    transformed_elements;}

template<
  template<typename...> class Container,
  typename ElementIn,
  typename ElementOut,
  typename Function>
ElementOut foldl(
  const ElementOut& accumulator,
  const Function& function,
  const Container<ElementIn>& elements)
{ return
    std::accumulate(
      begin(
        elements),
      end(
        elements),
      accumulator,
      function);}
template<
  template<typename...> class Container,
  typename ElementIn,
  typename ElementOut,
  typename Function> //:: (ElementOut, ElementIn) -> ElementOut
ElementOut accumulate(
  const Function& function,
  const Container<ElementIn>& elements)
{return
    foldl(
      ElementOut(),
      function,
      elements);}

template<
  template<typename...> class  Container,
  typename Element>
Element sum(
  const Container<Element>& elements,
  const Element& zero)
{return
    foldl(
      zero,
      [](const Element& total, const Element& element){
        return total + element;},
      elements);}

template<
  template<typename...> class  Container,
  typename Element>
Element sum(
  const Container<Element>& elements)
{return
    sum(
      elements,
      Element());}

template<
  template<typename...> class Container,
  typename Element>
double mean(
  const Container<Element>& elements)
{return
    (double) sum(elements, (Element) 0) / (double) elements.size();}

template<
  template<typename...> typename Container,
  typename ElementIn,
  typename ElementOut,
  typename Function>
ElementOut foldr(
  const Container<ElementIn>& elements,
  const Function& function,
  const ElementOut& accumulator)
{return
    std::accumulate(
      begin(
        elements),
      end(
        elements),
      accumulator,
      function);}

template<
  typename Predicate>
auto negation(
  const Predicate& p)
{ return
    [&p] (const auto& x) -> bool {return not p(x);};}

template<
  template<typename...> class Container,
  typename Element,
  typename Predicate>
Container<Element> filter(
  const Predicate& predicate,
  Container<Element> elements)
{ elements.erase(
    std::remove_if(
      begin(
        elements),
      end(
        elements),
      negation(
        predicate),
      end(
        elements)));
  return
    elements;}

template<
  typename Element,
  typename Predicate>
std::set<Element> filter(
  const Predicate& predicate,
  std::set<Element> elements)
{ std::set<Element> filtered_elements;
  std::copy_if(
    begin(
      elements),
    end(
      elements),
    std::inserter(
      filtered_elements,
      end(
        filtered_elements) ),
    predicate);
  return
    filtered_elements;}

template<
  template<typename...> class Container,
  typename ElementIn,
  typename Function,
  typename ContainerMapped= typename std::result_of<Function&(ElementIn)>::type,
  typename ElementOut= typename ContainerMapped::value_type>
Container<ElementOut> fmap(
  const Function& function,
  const Container<ElementIn>& elements_in)
{ Container<ElementOut> elements_out;
  for (const ElementOut& element : elements_in) {
    const Container<ElementOut>
      elements_mapped
      = function(element);
    elements_out.insert(
      end(
        elements_out),
      begin(
        elements_mapped),
      end(
        elements_mapped));}
  return elements_out;}



  
} /*namespace fxpp */
