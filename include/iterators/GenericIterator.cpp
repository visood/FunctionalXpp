#include <iterator>

template <typename ValueType>
class GenericIterator : public std::iterator<std::forward_iterator_tag, ValueType> {
public:

  virtual GenericIterator& start() = 0;
  virtual std::pair<ValueType, GenericIterator&> next() = 0;
  virtual bool done() = 0;

  GenericIterator& operator++() {
    next();
    return *this;
  }

  bool operator != (GenericIterator& that) {
    _itr != that.itr();
  }

  ValueType operator *() {
    return _crtval;
  }


private:
  ValueType _crtval;
}
