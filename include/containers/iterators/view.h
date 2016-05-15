
//template< typename BaseContainerType, typename ValueType>

template < template <typename... > BaseContainerType,
  typename BaseValueType, typename ValueType >
class View {
  using BaseItrType = typename BaseContainerType<BaseValueType>::iterator;
public:

  class iterator {
  public:

    using self_type = iterator;
    using self_reference = iterator&;
    using value_type = ValueType;
    using pointer = ValueType*;
    using reference = ValueType&;
    using iterator_category = std::iterator_traits<BaseItrType>::iterator_category;
    using difference_type = std::iterator_traits<BaseItrType>::difference_type;


    ValueType operator *() const {
      return _crtval;
    }

    const BaseContainerType<BaseValueType> const* base() {
      return _base;
    }

    size_t  basepos() {
      return std::distance(std::begin(*base), _bitr);
    }
    iterator operator ++() {
      std::pair<ValueType, BaseItrType> p =  View::next(_bitr);
      _bitr = p.second;
      if ( equals(_last) )
        _ended = true;
      _crtval = p.first;
      return *this;
    }

    bool operator equals (const iterator& that) const {
      _base == that.base() &&
        basepos() == that.basepos();
    }

    bool operator != (const iterator& that) const {
      !(equals (that));
    }

  private:
    const BaseContainerType<BaseValueType> const* _base = nullptr;
    BaseItrType _bitr;
    ValueType _crtval;
    iterator _last;
    _ended = false;
  };

  virtual static iterator start() const = 0;
  virtual static std::pair<ValueType, iterator>
    next(const iterator& bitr) const = 0;
  virtual static bool done(const iterator& itr ) const = 0;

  iterator begin(const BaseContainerType<BaseValueType>& base) {
    return iterator(std::begin(*_base));
  }

  iterator end(const BaseContainerType<BaseValueType>& base) {
    return iterator(std::end(*_base));
  }

  void for_each(ThisType& xs, auto func) {
    std::for_each(begin(xs.base()), end(xs.base()),
                  [] (auto const& x) { func(x); });
  }

private:
  const BaseContainerType<BaseValueType> const* _base = nullptr;
};

template < template <typename... > BaseContainerType,
                     typename BaseValueType, size_t k >
class KmerView : public View< BaseContainerType<BaseValueType>, std::array<BaseValueType, k> > {
  using ValueType = std::array<BaseValueType, k>;
public:
  static iterator start() const {
    return iterator(std::begin(*_base));
  }
  static std::pair<ValueType, iterator>
    next(const iterator& bitr) const {
    ValueType v;
    std::copy(bitr, bitr + k, std::begin(v));
    return std::make_pair(v, ++bitr);
  }
  static bool done(const iterator& itr) const {
    return return itr.ended();
  }
};
