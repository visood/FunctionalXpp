#include <iostream>
#include <sstream>
#include <algorithm>
#include <tuple>
#include "catch.hpp"
#include <string>
#include "RelationalDataBaseSim.h"

//a type that holds an usigned interger value
template<std::size_t> struct int_{};

template <class Tuple, size_t Pos>
    std::ostream& stream_tuple(std::ostream& out, const Tuple& t, int_<Pos> ) {
    out << std::get< std::tuple_size<Tuple>::value-Pos >(t) << ',';
    return stream_tuple(out, t, int_<Pos-1>());
}

template <class Tuple>
std::ostream& stream_tuple(std::ostream& out, const Tuple& t, int_<1> ) {
    return out << std::get<std::tuple_size<Tuple>::value-1>(t);
}

template <class... Args>
std::ostream& operator<<(std::ostream& out, const std::tuple<Args...>& t) {
    out << '(';
    stream_tuple(out, t, int_<sizeof...(Args)>());
    return out << ')';
}


template <typename First, typename... Rest>
    auto extended_tuple(std::tuple<Rest...> tup, First x) {
    return std::tuple_cat(tup, std::make_tuple(x));
}

template <typename T>
auto extended_tuple(std::tuple<T> t) {
    return t;
}


template < typename... Args>
auto extract_values(std::vector<std::string> vec, std::tuple<Args...>& t){
    if (vec.size() == 0) return t;
    DataType::get<T>(vec[0]);
}

template <typename... Args>
struct Row {
  int index = 0;
  std::tuple<Args...> data;
}

template <typename... Args>
class DatabaseTable {
  using RowType = std::tuple<Args...>;

 public:
 DatabaseTable(std::string tablename) : _name(tablename) {}

  int nrow() {return _data.size()};
  static constexpr int ncol() {return std::tuple_size<std::tuple<RowType> >::value;}

 private:
  const std::string _name;
  std::vector<RowType> _data;
}
