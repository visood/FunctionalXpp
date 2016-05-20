

template < template <typename... > typename BaseContainerType,
  typename BaseValueType, typename ValueType >
class View {
public:
  using base_container_type = BaseContainerType<BaseValueType>;
  using base_iterator_type = typename base_container_type::const_iterator;
  using value_type = ValueType;
  using self_type = View<BaseContainerType, BaseValueType, ValueType>;

  class iterator {
  public:

    using self_type = iterator;
    using self_reference = iterator&;
    using value_type = ValueType;
    using pointer = ValueType*;
    using reference = ValueType&;
    using iterator_category = typename std::iterator_traits<base_iterator_type>::iterator_category;
    using difference_type = typename std::iterator_traits<base_iterator_type>::difference_type;

  iterator(View::self_type const* const c,  const base_iterator_type& b) :
    _container(c), _bitr(b) {
      _ended =  c->done(b) ;
    }

    View::self_type const* const container() const {return _container;}
    const base_iterator_type& baseItr() const {return _bitr;}
    const bool ended() const {return _ended;}

    ValueType operator *() const {
      if (not _container->done(_bitr))
        return _crtval;
      else
        throw std::out_of_range("access a view element that ended");
    }

    iterator operator ++() {
      if (_ended) throw std::out_of_range("increment a view iterator that ended");
      std::pair<ValueType, base_iterator_type> p =  _container->next(_bitr);
      _crtval = p.first;
      if (_container->done(p.second)) {
        _ended = true;
      }
      _bitr = p.second;
      return *this;
    }

    const bool equals(iterator& that) const {
      if (_container != that.container()) return false;
      if (_ended) return not that.ended();
      if (that.ended()) return not _ended;
      return _container->equal(baseItr(), that.baseItr());
    }

    const bool operator ==(iterator& that) {
      return equals (that);
    }
    const bool operator !=(iterator& that) const {
      return not (equals(that));
    }

    size_t operator -(iterator& that) const {
      std::cout << "i exist to compute distance" << std::endl;
      return _bitr - that.baseItr();
    }
  private:
    View::self_type const* const _container = nullptr; //not owned
    base_iterator_type _bitr;
    ValueType _crtval;
    bool _ended = true;
  };

View(const base_container_type& b) :
  _base(b), _baseBegin(std::begin(b)), _baseEnd(std::end(b)) {}

View (const base_container_type& b, const base_iterator_type& bb, const base_iterator_type& be) :
  _base(b), _baseBegin(bb), _baseEnd(be) {}


  iterator start() ;
  std::pair<ValueType, base_iterator_type> next(const base_iterator_type& bitr) const;
  const bool done(const base_iterator_type& bitr ) const;
  const bool equal(const base_iterator_type& bitr1, const base_iterator_type& base2) const;

  size_t size() const { return _base.size(); }

  iterator begin(const base_container_type& base) {
    return iterator(this, std::begin(base));
  }

  iterator end(const base_container_type& base) {
    return iterator(this, std::end(base));
  }

  iterator begin() { return iterator(this, _baseBegin);}
  iterator end() { return iterator(this, _baseEnd);}

  void for_each(self_type& xs, auto func) {
    std::for_each(begin(xs.base()), end(xs.base()),
                  [&func] (auto const& x) { func(x); });
  }

private:
  const base_container_type& _base;
  const base_iterator_type& _baseBegin;
  const base_iterator_type& _baseEnd;
};

using SimpleView = View < std::vector, int, int >;

template<>
SimpleView::iterator SimpleView::start() {
  return iterator(this, _base.begin());
}

template<>
std::pair<SimpleView::value_type, SimpleView::base_iterator_type>
SimpleView::next(const base_iterator_type& bitr) const {
  return std::make_pair(*bitr, bitr + 1);
}

template<>
const bool SimpleView::done(const base_iterator_type& bitr) const {
  return not (bitr != _baseEnd);
}

template<>
const bool SimpleView::equal(const base_iterator_type& bitr1,
                             const base_iterator_type& bitr2) const {
  return not(bitr1 != bitr2);
}


template<typename T, size_t n>
using KmerView = View < std::vector, T, std::array<T, n> >;

template<typename T, size_t n>
typename KmerView<T, n>::iterator start() {
  return iterator(this, _base.begin());
}

template<typename T, size_t n>
std::pair<KmerView<T, n>::value_type, KmerView<T, n>::iterator>
next(const base_iterator_type& bitr) const {
  value_type v(bitr, bitr + n);
  return std::make_pair(v, bitr + 1);
}

template<typename T, size_t n>
bool done(const KmerView<T, n>::iterator& itr) const {
  return not (bitr != _baseEnd - n + 1);
}

template<typename T, size_t n>
const bool KmerView<T, n>::equal(const base_iterator_type& bitr1,
                                 const base_iterator_type& bitr2) const {
  return not(bitr1 != bitr2);
}

