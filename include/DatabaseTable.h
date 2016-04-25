#include <iostream>
#include <sstream>
#include <algorithm>
#include <tuple>
#include "catch.hpp"
#include <string>
#include "RelationalDataBaseSim.h"


//use templates to switch on desired return types

template<typename T>
struct switch_value {};

template <>
struct switch_value<int>
{
    enum class {value = 1};
};

template <>
struct switch_value<double>
{
    enum class {value = 2};
};

template <>
struct switch_value<std::string>
{
    enum class {value = 3};
};

template <typename T, typename R>
T get (R* res, int idx)
{
    switch (switch_value<T>::value)
    {
    case 1:
        return res->getInt32(idx);
    case 2:
        return res->getDouble(idx);
    case 3:
        return res->getString(idx);
    }
}


//to obtain the types in a tuple
template<class... Args>
struct type_list
{
    template <std::size_t N>
    using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
};

//a type that holds an usigned interger value
template<std::size_t N>
struct int_{
    using decr = int_<N-1>;
    using incr = int_<N+1>;
};

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

template<class Res, typename T>
T getValue(Res* res, int_<1>) {
    
}

template< class Res, size_t Pos, class... Args>
    getValue< type_list<Args...>::type<Pos> >(res)

template < typename... Args, class Res>
std::tuple<Args...> extract_values(Res* res){
    std::tuple<Args...> tup;
    auto x = std::get< std::tuple_size<Tuple>::value - Pos >(t);
    getValue(x, res, int_<Pos>());

}

template <typename... Args, typename R>
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
  static constexpr int ncol() {return std::tuple_size<std::tuple<RowType> >::value;}

  RowType read(R* res) {
      RowType tup;
      //set(res, tup, 1, int_<sizeof...(Args)>());
      set(res, tup, 1, int_<0>());
  }

  template<size_t IDX>
  void set(R* res, RowType& tup, int idx, int_<IDX>) {
      std::get<IDX>(tup) = get<type_list<Args...>::type<IDX> (res, idx);
      set(res, tup, idx + 1, int_<IDX + 1>());
  }

  template<>
  void set(R* res, RowType& tup, int idx, RowSize::decr > ) {
      std::get<1>(tup) = get<type_list<Args...>::type<sizeof...(Args)-1> (res, idx);
  }

  template<typename T>
  T get(R* res, std::size_t const idx) {
      return DatabaseTable::get_dispatcher<T>::impl(res, idx);
  }


 private:
  const std::string _name;
  std::vector<RowType> _data;

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
