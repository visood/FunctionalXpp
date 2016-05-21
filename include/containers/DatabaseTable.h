#pragma once

template <typename R, typename... Args>
struct Row {
  int index = 0;
  std::tuple<Args...> data;
};

template < typename... Args>
class DatabaseTable {
public:
  using RowType = std::tuple<Args...>;
  using RowSize = int_<sizeof...(Args)>;

 DatabaseTable(std::string tablename) : _name(tablename) {}

  uint nrow() {return (uint) _data.size(); }
  static constexpr int ncol() {return std::tuple_size<RowType>::value;}
  std::vector<RowType> const& data() {return _data;}


  template<class ResType>
  RowType read(ResType const res) {
    return getTupleValue<ResType, Args...>(res);
  }

  template<class ResType>
  void loadResult(ResType const res) {
    while(res->next()) {
      _data.push_back(read(res));
    }
  }

  template<class QueryType>
  void loadQuery(QueryType const query) {
    const auto res = query->execute();
    loadResult(res);
  }


private:
  const std::string _name;
  std::vector<RowType> _data;
  ArgPack<Args...> _argpack;
};
