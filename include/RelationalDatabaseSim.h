#include "catch.hpp"
#include "util.h"

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

template <typename... Args>
class Header {
public:
//Header(std::tuple<Args...> names): _names(names) {}
Header(Args... names): _names(std::make_tuple(names...)) {}

    std::string str() {
        return strtup(_names, int_<sizeof...(Args)>(), '\t');
    }

private:
    std::tuple<Args...> _names;
};

template <typename... Args>
Header<Args...> header(Args... names) {
    return Header<Args...>(names...);
}

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
  void load(const std::vector<std::vector<std::string> >& table) {
      for (auto row: table) insert(row);
  }
  void push_back(StrRdbRow row) {
    check_row_size(row);
    _data.push_back(row);
  }

  StrRowRdbTable const* next() const  {
    if (++_position > _data.size()) { return nullptr;}
    return this;
  }

  std::string getString(const uint index) const {  return _data[_position - 1].getString(index);}
  double getDouble(const uint index) const {  return _data[_position - 1].getDouble(index);}
  int getInt(const uint index) const {  return  _data[_position - 1].getInt(index);}

  void printCurrent() {
    _data[_position - 1].print();
  }
 protected:
  uint _ncol = 0;
  uint _nrow = 0;
  std::vector< StrRdbRow > _data;
  mutable uint _position = 0; //1 past the end
};

template <typename... Args>
class NamedStrRowRdbTable : public StrRowRdbTable {
    using string = std::string;
public:
NamedStrRowRdbTable(Args... hdrnames): StrRowRdbTable(std::tuple_size< std::tuple<Args...> >::value),
        _header(hdrnames...), _columnIndex(tupindexes(hdrnames...)) {}


    void load(const std::vector<std::vector<string> >& table) {
        StrRowRdbTable::load(table);
    }
    NamedStrRowRdbTable const* next() const {
        if(StrRowRdbTable::next()) return this;
        return nullptr;
    }
    string getString(const string& clm) const {
        return StrRowRdbTable::getString(_columnIndex.at(clm) + 1);
    }
    double getDouble(const string& clm) const {
        return StrRowRdbTable::getDouble(_columnIndex.at(clm) + 1);
    }
    int getInt(const string& clm) const {
        return StrRowRdbTable::getInt(_columnIndex.at(clm) + 1);
    }
private:
    Header<Args...> _header;
    const std::unordered_map< std::string, uint> _columnIndex;
};

template <class Table, typename... Args>
class NamedTable : public Table {
    using string = std::string;
public:
NamedTable(Args... hdrnames, const Table& table): _table(table), 
        _header(hdrnames...), _columnIndex(tupindexes(hdrnames...)) {}

    string getString(string clm) const { return _table.getString(_columnIndex.at(clm));}
    double getDouble(string clm) const { return _table.getDouble(_columnIndex.at(clm));}
    int getInt(string clm) const { return _table.getInt(_columnIndex.at(clm));}
private:
    Header<Args...> _header;
    const std::unordered_map<std::string, uint> _columnIndex;
    const Table& _table;
};

template <typename... Args, class Table>
NamedTable<Args...> namedTable(Args... clms, const Table& table) {
    return NamedTable<Args...>(clms..., table);
}

template <typename... Args, class Table>
    NamedTable<Args..., Table>* ptrNamedTable(Args... clms, const Table& table) {
    return new NamedTable<Args...>(clms..., table);
}

template <typename... Args>
NamedStrRowRdbTable<Args...>* ptrNamedStrRowRdbTable(Args... clms) {
    return new NamedStrRowRdbTable<Args...>(clms...);
}

template <typename... Args>
NamedStrRowRdbTable<Args...> namedStrRowRdbTable(Args... clms) {
    return NamedStrRowRdbTable<Args...>(clms...);
}

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