#include <iostream>
#include <sstream>
#include <algorithm>
#include <tuple>
#include "catch.hpp"
#include <string>
#include "RelationalDatabaseSim.h"



template <typename R, typename... Args>
struct Row {
  int index = 0;
  std::tuple<Args...> data;
};

template <typename... Args>
class DatabaseTable {
  using RowType = std::tuple<Args...>;
  using RowSize = int_<sizeof...(Args)>;

 public:
 DatabaseTable(std::string tablename) : _name(tablename) {}

  int nrow() {return _data.size(); }
  static constexpr int ncol() {return std::tuple_size<RowType>::value;}

#if 0
  {
  template<class ResType>
  RowType read(ResType* res) {
      RowType tup;
      set(res, tup, 1, int_<0>());
      return tup;
  }

  template<size_t IDX>
  void set(ResType* res, RowType& tup, int idx, int_<IDX>()) {
    std::get<IDX>(tup) = _vtArgs.get(res, idx, int_<IDX>());
    set(res, tup, idx + 1, int_<IDX + 1>());
  }

  template<>
  void set(ResType* res, RowType& tup, int idx, RowSize::decr()) {
    std::get<0>(tup) = _vtArgs.get(res, idx, int_<sizeof...(Args) - 1> );
  }

  template<typename T>
  struct DataType;
  template<typename T>
  T get(R* res, std::size_t const idx) {
      return DataType<T>(res, idx);
  }
}
#endif

  template <class ResType>
  RowType read(ResType const res) {
    //return readIdx<ResType>(res, int_<sizeof...(Args)>());
    return readIdx(res, int_<0>());
  }
  template <class ResType, std::size_t IDX>
    RowType readIdx(ResType const res, int_<IDX>) {
    //auto x = ArgPack<Args...>::Element(res, int_<std::tuple_size<RowType>::value - IDX >());
    //auto tup = readIdx(res, int_<IDX - 1>());
    auto x = ArgPack<Args...>::Element(res, int_<IDX>());
    auto tup = readIdx(res, int_<IDX + 1>());
    return std::tuple_cat(std::make_tuple(x), tup);
  }
  template <class ResType>
    RowType readIdx(ResType const res, int_<sizeof...(Args) - 1>) {
    //auto x = ArgPack<Args...>::Element(res, int_<std::tuple_size<RowType>::value - 1 >());
    auto x = ArgPack<Args...>::Element(res, int_<sizeof...(Args) - 1>());
    return std::make_tuple(x);
  }

 private:
  const std::string _name;
  std::vector<RowType> _data;
  ArgPack<Args...> _argpack;
};
