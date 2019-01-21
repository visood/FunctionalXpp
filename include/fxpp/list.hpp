/*
  Functional, persistent List.

  Vishal Sood
  20190118
 */
#pragma once
#include <iostream>
#include <memory>
#include <stdexcept>

namespace fxpp {namespace List {

template<typename ElemType> class List;

template<
  typename ElemType>
class ConsCell {
public:
  using this_type = ConsCell<ElemType>;

  ConsCell() = default;
  ConsCell(
    const ElemType& x,
    const std::shared_ptr<this_type>& nextPtr
  ):
    _elemPtr(std::make_unique<ElemType>(x)),
    _nextPtr(nextPtr)
  {
    //*_elemPtr= x;
  }
  ConsCell(
    const ElemType& x
  ):
    ConsCell(x, nullptr)
  {}
  ConsCell(
    const ElemType& x,
    const this_type& that
  ) : ConsCell(x,
               std::make_shared<this_type>(that))
  {}
  ConsCell(
    const this_type& that
  ):
    _elemPtr(that.empty() ? nullptr : std::make_unique<ElemType>(that.elem())),
    _nextPtr(that.nextPtr())
  {}
  ConsCell(
    this_type&& that
  ) noexcept :
    _elemPtr(that.empty() ? nullptr : std::make_unique<ElemType>(that.elem())),
    _nextPtr(std::move(that.nextPtr()))
  {}

  ~ConsCell()
  {}

  this_type operator= (
    const this_type& that) 
  { std::cout << "assign with mutable = " << std::endl;
    _elemPtr = std::make_unique<ElemType>(that.elem());
    _nextPtr = that.nextPtr();
    return *this;}
        
  this_type operator= (
    const this_type& that) const
  { std::cout << "assign with const = " << std::endl;
    return 
      ConsCell<ElemType>(that);}
        
  const std::unique_ptr<ElemType>& elemPtr() const {return _elemPtr;}
  const ElemType&  elem() const {return *_elemPtr;}
  const ElemType& get() const {return *_elemPtr;}
  const this_type& next() const {return *_nextPtr;}
  const std::shared_ptr<this_type>& nextPtr() const {return _nextPtr;}
  bool empty() const {return _elemPtr? false: true;}

  friend class List<ElemType>;
private:
  std::unique_ptr<ElemType> _elemPtr;
  std::shared_ptr<this_type> _nextPtr;
};

template<
  typename ElemType>
const ConsCell<ElemType> Nil; /*can we make this constexpr?*/


template<typename ElemType>
class List{
public:
  using ConsCellType = ConsCell<ElemType>;

  List(
  ) : _headCellPtr(std::make_shared<ConsCellType>(ConsCellType()))
  {
    //.*_headCellPtr = ConsCellType();
  }
  List(
    const std::shared_ptr<ConsCellType>& headCellPtr
  ): _headCellPtr(headCellPtr)
  {}

  bool empty() const
  { return
      _headCellPtr->empty();}
  bool nil() const
  { return
      _headCellPtr->empty();}
  int size() const
  { return
      empty() ? 0 : 1 + tail().size();}
  const ElemType& head() const
  { return
      *(_headCellPtr->_elemPtr);}

  List<ElemType> tail() const 
  { if (empty()) 
      throw std::out_of_range(
        "tail called on an empty list!!!");
    return
      List<ElemType>(_headCellPtr->_nextPtr);}
  List<ElemType> cons(
    const ElemType& x) const 
  { return
      List<ElemType>(
        std::make_shared<ConsCellType>(
          ConsCellType(
            x, _headCellPtr)) );}
  void printCellUsage() const
  { if (empty())
      std::cout << "done" << std::endl;
    else {
      std::cout << "head with element "
                << *(_headCellPtr->_elemPtr) << ", "
                << "cell usage: " << _headCellPtr.use_count() << ", "
                << "( head-cell's next has usage "
                << _headCellPtr->_nextPtr.use_count()
                << ");";
      tail().printCellUsage();}}
private:
  std::shared_ptr<ConsCellType> _headCellPtr;
};

template<
  typename ElemType>
const ElemType& head(
  const List<ElemType>& l)
{ return
    l.head();}
template<
  typename ElemType>
const List<ElemType> tail(
  const List<ElemType>& l)
{ return
    l.tail();}
template<
  typename ElemType>
List<ElemType> cons(
  const ElemType& x,
  const List<ElemType>& ys)
{ return
    ys.cons(x);}
template<
  typename ElemType>
List<ElemType> make_list(const ElemType& x)
{ return
    cons(
      x,
      List<ElemType>());}
template<
  typename HeadType,
  typename... TailTypes>
List<HeadType> make_list(
  const HeadType& head_element,
  TailTypes... tail_elements)
{ return
    cons(
      head_element,
      make_list(tail_elements...));}
}/*namespace List*/ } /*namespace fxpp*/

template<
  typename ElemType,
  typename ListType = fxpp::List::List<ElemType> >
inline const ListType operator >> (
  const ElemType& head,
  const ListType& tail)
{ return
    tail.cons(head);}
