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
      //std::get<IDX>(tup) = get<type_list<Args...>::type<IDX> (res, idx);
    std::get<IDX>(tup) = _vtArgs.get(res, idx, int_<IDX>());
    set(res, tup, idx + 1, int_<IDX + 1>());
  }

  template<>
  void set(ResType* res, RowType& tup, int idx, RowSize::decr()) {
      //std::get<1>(tup) = get<type_list<Args...>::type<sizeof...(Args)-1> (res, idx);
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
  RowType read(ResType* res) {
    return read(res, 1, int_<0>());
  }

  template <class ResType, size_t IDX>
  RowType read(ResType* res, int idx, int_<IDX>()) {
    //ArgPack<Args...>::type<IDX> x = ArgPack<Args...>::Element<IDX>::get(res, idx);
    auto x = Element<ResType, Args...>::get<IDX>(res, idx);
    auto tup = read(res, idx + 1, int_<IDX>::incr());
    return std::tuple_cat(std::make_tuple(x), tup);
  }

  template <class ResType>
    RowType read(ResType* res, int idx, int_<sizeof...(Args) - 1>()) {
    auto x = Element<ResType, Args...>::get<sizeof...(Args) - 1>(res, idx);
    return std::make_tuple(x);
    //return std::make_tuple(Element<ResType>::getValue<IDX>(res, idx));
  }


 private:
  const std::string _name;
  std::vector<RowType> _data;

  //ArgPack<Args...> _argpack;

  // ValueType<Args...> _vtArgs();
};



/*
template<typename R>
struct DatabaseTable::DataType<int> {
  static int operator()(R* res, std::size_t const idx) {
        return res->getInt32(idx);
  }
};

template<typename R>
struct DatabaseTable::DataType<double> {
  static double operator()(R* res, std::size_t const idx) {
        return res->getDouble(idx);
  }
};

template<typename R>
struct DatabaseTable::DataType<std::string> {
  static std::string operator()(R* res, std::size_t const idx) {
        return res->getString(idx);
  }
};

*/
