#include <iostream>
#include <sstream>
#include <algorithm>
#include <tuple>
#include "catch.hpp"
#include <string>

template<typename T>
struct DType {};

namespace DataType {
    const DType<double> Double;
    const DType<std::string> String;
    const DType<int> Integer;

    template<typename T, typename S>
    T convert(const S& data) {
        std::istringstream i(data);
        T x;
        if (!(i >> x)) throw std::invalid_argument("invalid data");
        return x;
    }
    template<typename T>
    T get(const std::string& data) {
        std::istringstream i(data);
        T x;
        if (!(i >> x)) throw std::invalid_argument("invalid data string: " + data);
        return x;
    }

};

template<typename T>
struct DataTypeValue {
DataTypeValue(const T& v) : value(v) {}
    DataTypeValue() = default;
    T value;
};

template<typename T>
T getValue(T& dtype, const std::string& datastr) {
    std::istringstream i(datastr);
    T x;
    if (!(i >> x)) throw std::invalid_argument("invalid data string" + datastr);
    dtype = x;
    return dtype;
}

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
}

class StrRdbRow {
  //using DataStr = std::vector<std::string>;

 public:
 StrRdbRow(std::vector<std::string> row) : _data(row);

  int nfields() const { return _data.size();}
  void check_index(int index) const {
    if (index >= _data.size())
      throw std::invalid_argument("requested index " +
                                  DataType::convert<std::string, int>(_data.size()) +
                                  " is larger than row size " +
                                  DataType::convert<std::string, int>(_row.size()));
  }

  std::string getString(int index) const { check_index; return _data[index];}
  double getDouble(int index) const { check_index; return DataType::convert<double, std::string>(_data[index]);}
  int getInt(int index) const { check_index; return  DataType::convert<int, std::string>(_data[index]);}

 private:
  std::vector<std::string> _data;
};

class StrRowRdbTable {
 public:
 StrRowRdbTable(int ncol) : _ncol(ncol), _nrow(0);

  template<typename Sized>
  void check_row_size(const Sized& v) {
    if (v.size() != col)
      throw std::invalid_argument("A row of " + DataType::convert<std::string, int>(_ncol) +
                                  " cannot be extracted from a vector of length " +
                                  DataType::convert<std::string, int>(v.size()));
    }
  void insert(std::vector<std::string> row) {
    push_back( StrRdbRow(row));
  }
  void insert(StrRowRow row) {
    push_back( StrRdbRow(row));
  }
  void push_back(StrRdbRow row) {
    check_row_size(row);
    _data.push_back(row);
  }
 private:
  int _ncol = 0;
  int _nrow = 0;
  std::vector< StrRdbRow > _data;
}
