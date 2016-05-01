
#include <iterator>

template <typename Iterable>
class Kmer {
  using ValueType = Iterable;
  using BaseItrType = typename Iterable::iterator;
public:
Kmer(uint k, Iterable xs): _k(k), _xs(xs) {}
  uint k() { return _k;}
  uint size() { return (uint) (_xs.size() - _k + 1);}
  class iterator : public std::iterator<std::forward_iterator_tag, ValueType>  {
  public:
  iterator(const BaseItrType bitr,
           const BaseItrType xitr,
           const BaseItrType eitr,
           const uint k ): _bitr(bitr), _xitr(xitr), _eitr(eitr), _k(k) {}
  iterator(const BaseItrType bitr,
           const BaseItrType eitr,
           const uint k ): _bitr(bitr), _xitr(bitr), _eitr(eitr), _k(k) {}
    Iterable operator*() {
      Iterable elem(_k);
      std::copy(_xitr, _xitr + _k, std::begin(elem));
      return elem;
    }
    iterator& operator++() {
      _xitr++;
      return *this;
    }
    BaseItrType xitr() { return _xitr;}
    bool operator!=(iterator& that) {
      return _xitr != that.xitr();
    }

    const BaseItrType& start() { _xitr = _bitr; return _bitr;}
    BaseItrType& next() { _xitr++; return _xitr;}
    bool done() { return _xitr < _eitr;}

  private:
    const BaseItrType _bitr;
    BaseItrType _xitr;
    const BaseItrType _eitr;
    const uint _k;
  };

  iterator begin() {return iterator(std::begin(_xs), std::begin(_xs),  std::end(_xs), _k);}
  iterator end() {return iterator(std::begin(_xs), std::end(_xs) - _k + 1, std::end(_xs) - _k + 1, _k);}
private:
  const uint _k;
  Iterable _xs;
};

template < typename Iterator, typename VoidOpr >
void for_each(Iterator& itr, const VoidOpr& func) {
  while (not itr.done() ) {
    func(*itr);
    ++itr;
  }
}
