#include <iostream>
#include <sstream>
#include <algorithm>
#include <tuple>
#include "catch.hpp"
#include <string>
#include <limits>

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

class StrRdbRow {
  //using DataStr = std::vector<std::string>;

 public:
StrRdbRow(std::vector<std::string> row) : _data(row) {}

  uint nfields() const { return (uint) _data.size();}
  uint size() const { return (uint) _data.size();}
  void check_index(uint index) const {
    if (index > (uint) _data.size())
      throw std::invalid_argument("requested index " +
                                  DataType::convert<std::string, uint>(index) +
                                  " is larger than row size " +
                                  DataType::convert<std::string, uint>((uint) _data.size()));
  }

  std::string getString(uint index) const { check_index(index); return _data[index - 1];}
  double getDouble(uint index) const { check_index(index); return DataType::convert<double, std::string>(_data[index - 1]);}
  int getInt(uint index) const { check_index(index); return  DataType::convert<int, std::string>(_data[index - 1]);}

  void print() {
    for (auto f: _data)
      std::cout << f << ", ";
    std::cout << std::endl;
  }

 private:
  std::vector<std::string> _data;
};

class StrRowRdbTable {
 public:
StrRowRdbTable(uint ncol) : _ncol(ncol), _nrow(0) {}
StrRowRdbTable(uint ncol, const std::vector< std::vector<std::string> >& table) : _ncol(ncol), _nrow((uint) table.size()) {
    for (auto row: table) {
      insert(row);
    }
  }

  template<typename Sized>
  void check_row_size(const Sized& v) const {
    if (v.size() != _ncol)
      throw std::invalid_argument("A row of " + DataType::convert<std::string, int>(_ncol) +
                                  " cannot be extracted from a vector of length " +
                                  DataType::convert<std::string, int>(v.size()));
  }
  uint size() const { return (uint) _data.size();}
  uint position() const { return _position;}
  void insert(std::vector<std::string> row) {
    push_back( StrRdbRow(row));
  }
  void insert(StrRdbRow row) {
    push_back( StrRdbRow(row));
  }
  void push_back(StrRdbRow row) {
    check_row_size(row);
    _data.push_back(row);
  }

  StrRowRdbTable* next()  {
    if (++_position > _data.size()) { return nullptr;}
    return this;
  }

  std::string getString(uint index) const {  return _data[_position - 1].getString(index);}
  double getDouble(uint index) const {  return _data[_position - 1].getDouble(index);}
  int getInt(uint index) const {  return  _data[_position - 1].getInt(index);}

  void printCurrent() {
    _data[_position - 1].print();
  }
 private:
  uint _ncol = 0;
  uint _nrow = 0;
  std::vector< StrRdbRow > _data;
  uint _position = 0; //1 past the end
};


template<typename ResType>
class IterableRDB {
public:
IterableRDB(ResType* res) : _start(res) {}

  class Iterator {
  public:
  Iterator(ResType* res, int p) : _current(res), _position(p) {}

    using self_type = Iterator;
    using self_reference = Iterator&;
    using iterator_category = std::input_iterator_tag;
    using value_type = ResType*;
    using pointer = value_type const*;
    using reference = value_type const&;
    using difference_type = int32_t;

    uint position() const {return _position;}
    reference operator*() const { return _current;}
    pointer operator->() const {return &_current;}
    self_reference operator++() {
      if(not _current->next())
        _position = std::numeric_limits<int>::max();
      else
        _position += 1;
      return *this;
    }
    bool operator !=(const Iterator& that) const {
      return _position != that.position();
    }
  private:
    ResType* _current;
    uint _position = 0;
  };

  const Iterator begin() const {
    return ++Iterator(_start, 0);
  }

  const Iterator end() const {
    return Iterator(nullptr, std::numeric_limits<int>::max());
  }

private:
  ResType* _start = nullptr;
};
