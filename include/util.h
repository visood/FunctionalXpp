#include <limits>
#include <dirent.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <string>
#include <limits>
#include <tuple>
#include<unordered_map>


std::string wordyOneToNine(uint x) {
  if (x > 9) throw std::invalid_argument(
      "wordyOneToNine cannot wordify number larger than 9");
  if (x == 0) throw std::invalid_argument(
      "wordyOneToNine cannot wordify 0");
  switch (x) {
  case 9:
    return "nine";
  case 8:
    return "eight";
  case 7:
    return "seven";
  case 6:
    return "six";
  case 5:
    return "five";
  case 4:
    return "four";
  case 3:
    return "three";
  case 2:
    return "two";
  case 1:
    return "one";
  case 0:
    return "";
  }
  return "";
}

std::string wordyInteger(uint x) {
  if (x > 1000) throw std::invalid_argument(
      "wordInteger cannot wordify numbers that are larger than 1000");
  std::string wordy;
  if (x > 100) wordy = wordyOneToNine(x/100) + " hundred";
  if (x > 100 and x % 100 > 0)  wordy += " and ";
  x = x % 100;
  switch (x / 10) {
  case (9):
    wordy += "ninety";
    break;
  case (8):
    wordy += "eighty";
    break;
  case (7):
    wordy += "seventy";
    break;
  case (6):
    wordy += "sixty";
    break;
  case (5):
    wordy += "fifty";
    break;
  case (4):
    wordy += "forty";
    break;
  case (3):
    wordy += "thirty";
    break;
  case (2):
    wordy += "twenty";
    break;
  case (1):
    switch (x % 10) {
    case (9):
      wordy += "nineteen";
      return wordy;
    case (8):
      wordy += "eighteen";
      return wordy;
    case (7):
      wordy += "seventeen";
      return wordy;
    case (6):
      wordy += "sixteen";
      return wordy;
    case (5):
      wordy += "fifteen";
      return wordy;
    case (4):
      wordy += "fourteen";
      return wordy;
    case (3):
      wordy += "thirteen";
      return wordy;
    case (2):
      wordy += "twelve";
      return wordy;
    case (1):
      wordy += "eleven";
      return wordy;
    }
  case (0):
    break;
  }
  if (x % 10 == 0) return wordy;
  if (x > 10 ) wordy += " ";
  wordy += wordyOneToNine( x % 10 );
  return wordy;
}

//a type that holds an unsigned integer value
template<std::size_t N>
struct int_{};

template<std::size_t N>
struct decr_{
  using type = typename int_<N-1>::type;
};

template<std::size_t N>
struct incr_{
  using type = typename int_<N+1>::type;
};

template <class Tuple, size_t Pos>
std::string strtup(const Tuple& t, int_<Pos>, const char delim = ',') {
    std::stringstream s;
    s << std::get<std::tuple_size<Tuple>::value - Pos > (t) << delim;
    return  s.str() + strtup(t, int_<Pos-1>(), delim);
}

template <class Tuple>
std::string strtup(const Tuple& t, int_<1>, const char delim = ",") {
    std::stringstream s;
    s << std::get<std::tuple_size<Tuple>::value - 1>(t) << delim;
    std::string st = s.str();
    st.pop_back();
    return st;
}


//for a tuple header we can create a map from the tuple elements to their index
template <class Tuple, size_t Pos>
std::unordered_map<std::string, uint> tupindexes(const Tuple& t, int_<Pos>) {
    auto map1 = tupindexes(t, int_<Pos-1>());
    map1[std::get<Pos - 1>(t)] = Pos - 1;
    return map1;
}

template <class Tuple>
std::unordered_map<std::string, uint> tupindexes(const Tuple& t, int_<1>) {
    std::unordered_map<std::string, uint> map;
    map[std::get<0>(t)] = 0;
    return map;
}

template <typename... Args>
std::unordered_map<std::string, uint> tupindexes(Args... names) {
    auto tup = std::make_tuple(names...);
    return tupindexes(tup, int_<sizeof...(Args)>());
}

//use templates to switch on desired return types

template<typename T>
struct dispatch_value {};

template <>
struct dispatch_value<int>
{
    enum {value = 1};
};

template <>
struct dispatch_value<double>
{
    enum {value = 2};
};

template <>
struct dispatch_value<std::string>
{
    enum {value = 3};
};

//to obtain the types in a tuple
template<class... Args>
struct type_list
{
    template <std::size_t N>
    using type = typename std::tuple_element<N, std::tuple<Args...> >::type;
};

template <typename T>
struct _type_{};

template<typename... Args>
struct ArgPack {
  template <std::size_t N>
  using type = typename std::tuple_element<N, std::tuple<Args...> >::type;

  template <class ResType>
  static int getValue(ResType res, int idx, _type_<int>) {
    //std::cout << "getInt(" << idx + 1 << ")"
    //<< res->getInt(idx + 1) << std::endl;
    return res->getInt(idx + 1);
  }

  template <class ResType>
  static double getValue(ResType res, int idx, _type_<double>) {
    //std::cout << "getDouble(" << idx + 1 << ")"
    //<< res->getDouble(idx + 1) << std::endl;
    return res->getDouble(idx + 1);
  }

  template <class ResType>
  static std::string getValue(ResType res, int idx, _type_<std::string>) {
    //std::cout << "getString(" << idx + 1 << ")"
    //<< res->getString(idx + 1) << std::endl;
    return res->getString(idx + 1);
  }

  template <class ResType, std::size_t IDX>
    static type<IDX> Element(ResType res, int_<IDX>) {
    //std::cout << "Element(" << IDX << ")"
    //<< getValue(res, IDX, _type_< type<IDX> >()) << std::endl;
    return getValue(res, IDX, _type_< type<IDX> >());
  }
};



template<class ResType, typename First, typename... Rest>
static std::tuple<First, Rest...> getTupleValue(ResType res, int offset = 0) {
  auto x = ArgPack<>::getValue(res, offset, _type_<First>());
  auto tup = getTupleValue<ResType, Rest...>(res, offset + 1);
  return std::tuple_cat( std::make_tuple(x), tup);
}
template<class ResType>
static std::tuple<> getTupleValue(ResType, int) {
  return std::tuple<>();
}


template<typename... Args>
struct ValueType {
  template <std::size_t N>
  using type = typename std::tuple_element<N, std::tuple<Args...> >::type;

  template<class Res, size_t IDX>
  type<IDX> get(Res* res, int idx, int_<IDX>) {
    switch (dispatch_value< type<IDX> >::value)
    {
    case 1:
      return res->getInt(idx);
    case 2:
      return res->getDouble(idx);
    case 3:
      return res->getString(idx);
    }
  }
};

//stream the contents of a tuple
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



template < typename T >
T trivial() ;

template<>
std::string trivial() { return "";}
template<>
uint trivial() { return (uint) 0;}
template<>
int trivial() { return 0;}
template<>
double trivial() { return 0.0;}

template < typename T >
class Maybe {
  virtual bool isValid() = 0;
  virtual T get() = 0;
};

template < typename T >
class None : public Maybe<T> {
public:
  bool isValid() { return false;}
  T get() { throw std::invalid_argument("None does not contain any elements"); }
};

template < typename T >
class Valid : public Maybe<T> {
public:
Valid(const T& e) : _elem(e) {}
  bool isValid() {return true;}
  const T get() const { return _elem; }
private:
  const T _elem;
};

template<typename... As> struct Tower;

template<typename A> struct Tower<A> {
    Tower(A i) : _value(i) {}

  void operator()(A a) const { std::cout << _value + a << std::endl; }

    private:
        A _value;
};

template<typename First, typename... Rest>
struct Tower<First, Rest...> : Tower<First>, Tower<Rest...>
{
    using Tower<First>::operator();
    using Tower<Rest...>::operator();

    Tower(First f, Rest... rs) :
        Tower<First>(f), Tower<Rest...>(rs...) {}
};
template <typename... Fs>
Tower<Fs...> make_tower(Fs... fs) { return {fs...}; }

