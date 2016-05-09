#include <iterator>

template < template <typename...> typename ValueType>
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


protected:
  ValueType _crtval;
};



template< typename T, size_t k >
using Kmer = std::array<T, k>

template< template<typename...> typename BaseContainerType,
          typename BaseValueType,
          size_t k >
class KmerItr : public GenericIterator< Kmer<BaseValueType, k> > {
  using ValueType = Kmer<BaseValueType, k>;

}



class KmerItr : public GenericIterator<std::array<BaseValueType, k> {
  using ValueType = std::array<BaseValueType, k>;
public:
  KmerItr(const BaseItrType& bitr, const BaseValueType& eitr):
    _bitr(bitr), _xitr(bitr), _eitr(eitr) {}

  KmerItr& start() {
    return this;
  }

  std::pair<ValueType, KmerItr&> next() {
    _xitr++;
    ValueType crtval = _crtval;
    std::copy(_xitr + _xitr + k, std::begin(_crtval));
    return std::make_pair(crtval, *this);
  }

  bool done() {
    return std::distance(_xitr, _eitr) < k;
  }

private:
  const BaseItrType _bitr = nullptr;
  BaseItrType _xitr = nullptr;
  const BaseItrType _eitr = nullptr;
};



template < template<typename...> typename BaseContainerType,
           template<typename...> typename ValueType >
class Morphism {
  template <typename... Args>
  using BaseIterator = BaseContainerType<Args...>::iterator;
public:
  template<typename... Args>
  Morphism(const BaseContainerType<Args...>& base) : _base(base) {}

  class iterator {
    using self_type = iterator;
    using self_reference = iterator&;
    using value_type = ValueType;
    using pointer = ValueType*;
    using reference = ValueType&;
    using iterator_category = std::iterator_traits<BaseIterator>::iterator_category;
    using difference_type = std::iterator_traits<BaseIterator>::difference_type;

    ValueType operator*() {
      return _crtval;
    }
    iterator& operator++() {
      next();
      return *this;
    }
    bool !=(const iterator& that) {
      _base != that.base() or

    }
  };

private:
  template<typename... Args>
  const BaseContainerType<Args...>& _base;
};

