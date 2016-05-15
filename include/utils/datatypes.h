#pragma once

template<typename T>
struct DType {};

namespace DataType {
  const DType<double> Double;
  const DType<std::string> String;
  const DType<int> Integer;

  template<typename T, typename S>
    T convert(const S& data) {
    std::stringstream i;
    i << data;
    T x;
    if (!(i >> x)) throw std::invalid_argument("invalid data");
    return x;
  }
  template<typename T>
    T convert(const std::string& data) {
    std::stringstream i;
    i << data;
    T x;
    if (!(i >> x)) throw std::invalid_argument("invalid data string: " + data);
    return x;
  }

  template<typename T, typename S>
    T getValue(T& t, const S& s) {
    t = convert<T, S>(s);
    return t;
  }

};

#if 0
template<std::size_t>
class DataStr {
public:
DataStr(int n) : _size(n) {}

  template<typename T>
    int insert(const T& x) {
    std::istringstream i(x);
    std::string x;
    if ()
      }
};
#endif

