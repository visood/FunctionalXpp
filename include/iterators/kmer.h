
#include <array>
#include <iterator>

template <typename Iterable, typename ValueType, size_t k>
class Kmer {
  //using ValueType = Iterable;
  using BaseItrType = typename Iterable::iterator;
public:
Kmer( Iterable xs): _xs(xs) {}
  //uint k() { return _k;}
  uint size() { return (uint) (_xs.size() - k + 1);}
  class iterator : public std::iterator<std::forward_iterator_tag, ValueType>  {
  public:
  iterator(const BaseItrType bitr,
           const BaseItrType xitr,
           const BaseItrType eitr)
    : _bitr(bitr), _xitr(xitr), _eitr(eitr) {}
  iterator(const BaseItrType bitr,
           const BaseItrType eitr)
    : _bitr(bitr), _xitr(bitr), _eitr(eitr) {}

    std::array<ValueType, k> operator*() {
      std::array<ValueType, k>  elem;
      std::copy(_xitr, _xitr + k, std::begin(elem));
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
    //const uint _k;
  };

  iterator begin() {return iterator(std::begin(_xs), std::begin(_xs),  std::end(_xs));}
  iterator end() {return iterator(std::begin(_xs), std::end(_xs) - k + 1, std::end(_xs) - k + 1);}
private:
  //const uint _k;
  Iterable _xs;
};

template < typename Iterator, typename VoidOpr >
void for_each(Iterator& itr, const VoidOpr& func) {
  while (not itr.done() ) {
    func(*itr);
    ++itr;
  }
}
