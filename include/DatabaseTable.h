#include <iostream>
#include <sstream>
#include <algorithm>
#include <tuple>
#include "catch.hpp"
#include <string>
#include "RelationalDataBaseSim.h"



template <typename R, typename... Args>
struct Row {
  int index = 0;
  std::tuple<Args...> data;
}

template <typename... Args>
class DatabaseTable {
  using RowType = std::tuple<Args...>;
  using RowSize = int_<sizeof...(Args)>;

 public:
 DatabaseTable(std::string tablename) : _name(tablename) {}

  int nrow() {return _data.size()};
  static constexpr int ncol() {return std::tuple_size<RowType>::value;}

  RowType read(R* res) {
      RowType tup;
      set(res, tup, 1, int_<0>());
      return tup;
  }

  template<size_t IDX>
      void set(R* res, RowType& tup, int idx, int_<IDX>()) {
      //std::get<IDX>(tup) = get<type_list<Args...>::type<IDX> (res, idx);
      std::get<IDX>(tup) = _args.get(res, int_<IDX>());
      set(res, tup, idx + 1, int_<IDX + 1>());
  }

  template<>
  void set(R* res, RowType& tup, int idx, RowSize::decr()) {
      //std::get<1>(tup) = get<type_list<Args...>::type<sizeof...(Args)-1> (res, idx);
      std::get<0>(tup) = _args.get(res, int_<sizeof...(Args) - 1> );
  }

  template<typename T>
  T get(R* res, std::size_t const idx) {
      return DatabaseTable::get_dispatcher<T>::impl(res, idx);
  }


 private:
  const std::string _name;
  std::vector<RowType> _data;

  ValueType<Args...> _args();
  template<typename T>
  struct get_dispatcher;
};

template<typename R>
struct DatabaseTable::get_dispatcher<int> {
    static int impl(R* res, std::size_t const idx) {
        return res->getInt32(idx);
    }
};

template<typename R>
struct DatabaseTable::get_dispatcher<double> {
    static double impl(R* res, std::size_t const idx) {
        return res->getDouble(idx);
    }
};

template<typename R>
struct DatabaseTable::get_dispatcher<std::string> {
    static std::string impl(R* res, std::size_t const idx) {
        return res->getString(idx);
    }
};
