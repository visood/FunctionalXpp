#pragma once

template <typename... Args>
class TupleFrame {
public:
  using RowType = std::tuple<Args...>;
  using RowSize = int_<sizeof... (Args)>;


  template <typename ResType>
    TupleFrame(ResType res) {loadResult(res);}

  TupleFrame() = default;

  uint nrow() {return (uint) _data.size();}

  static constexpr uint ncol() {return (uint) std::tuple_size<RowType>::value;}
  const std::vector<RowType>& operator()() {return _data;}

  RowType operator[](uint i) {return _data[i];}

  template<class ResType>
  RowType readRow(ResType res) {
    return getTupleValue<ResType, Args...>(res);
  }

  template<class ResType>
  void loadResult(ResType res) {
    while(res->next()) {
      _data.push_back(readRow(res));
    }
  }

  template<class ResType>
  ResType loadNext(ResType res, uint N=10000) {
    uint n = 0;
    while(res->next() and n != N) {
      _data.push_back(readRow(res));
      ++n;
    }
    return res;
  }

private:
  std::vector<RowType> _data;
};


//another version

template <typename... Args>
struct Read{
  template <typename ResType>
  static std::tuple<Args...> result(ResType res) {
    return getTupleValue<ResType, Args...>(res);
  }
};

template <typename ResType, typename... Args>
struct Reader {
  std::tuple<Args...> operator() (ResType res) {
    return getTupleValue<ResType, Args...>(res);
  }
};

#if 0
template <typename T, typename S>
  struct Mapper {
    using type_from = S;
    using type_to = T;
  };

//template < typename Reader, typename Mapper>
template< typename To, typename... From >
class TupleFrame {
public:
    TupleFrame(Read<S> r, Mapper<T, S> m) :
  _read(r), _mapped(m) {}

  template< class ResType>
  auto collect(ResType res) {
    if (res)
    std::vector<To>> df;
    while (res->next()) {
      df.push_back(_mapped(_read(res)));
    }
  }

  template<TupMapper func, typename U >
  TupleFrame< U, S >  Map(TupMapper func, Mapper<U, T>) {
    return TupleFrame(_read, [] (auto tup) {return func(_mapped(tup));});
  }


private:
  Read<S> _read;
  Mapper<T, S> _mapped;
}
#endif
