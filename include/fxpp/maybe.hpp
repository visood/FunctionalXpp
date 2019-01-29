/*
  Type Maybe that wraps an element or nothing.
  Think of it as a box that may contain up to only one element.

  Vishal Sood
  2019018
 */
#include <iostream>
#include <memory>
#include <stdexcept>

namespace fxpp { namespace Maybe{
template<
  typename ElemType>//element type
class Maybe{
public:
  Maybe() = default;
  Maybe(const ElemType& x):
    _elem(std::make_unique<ElemType>())
  {
    *_elem = x;
    empty = false;
  }

  bool empty = true;

  const ElemType& get() const {
    if (not _elem)  {
      throw std::out_of_range("Maybe is None");}
    return *_elem;}

  template<
    typename Function,
    typename S = typename std::result_of<Function&(ElemType)>::type>
  Maybe<S> map(
    const Function& func) const
  {return
      empty ? Maybe<S>() : Maybe<S>(func(*_elem));}
  template<
    typename Predicate>
  Maybe<ElemType> filter(
    const Predicate& pred) const
  {return
      empty ? Maybe<ElemType>() : (
        pred(*_elem) ? Maybe<ElemType>(*_elem) : Maybe<ElemType>());}
private:
  std::unique_ptr<ElemType> _elem;
};

// template<
//   typename ElemType>
// Maybe<ElemType> None = Maybe<ElemType>();

template<
  typename Element>
Maybe<Element> None()
{return
    Maybe<Element>();}

template<
  typename ElemType>
Maybe<ElemType> Some(const ElemType& x) {
  return Maybe<ElemType>(x);}
} /*namespace Maybe */} /*namespace fxpp*/ 
