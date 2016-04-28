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

template < typename... Args>
class DatabaseTable {
  using RowType = std::tuple<Args...>;
  using RowSize = int_<sizeof...(Args)>;

 public:
 DatabaseTable(std::string tablename) : _name(tablename) {}

  int nrow() {return _data.size(); }
  static constexpr int ncol() {return std::tuple_size<RowType>::value;}


  template<class ResType>
  RowType read(ResType const res) {
    return getTupleValue<ResType, Args...>(res, 0);
  }

private:
  const std::string _name;
  std::vector<RowType> _data;
  ArgPack<Args...> _argpack;
};
