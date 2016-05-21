#pragma once

template <typename... Args>
class DataFrame {
public:
  using RowType = std::tuple<Args...>;
  using RowSize = int_<sizeof... (Args)>;

  template <size_t j>
    using atype = typename type_list<Args...>::template type<j>;

  template <size_t j>
    using ctype = std::vector< atype<j> >;

  template <typename ResType>
    DataFrame(ResType res) {loadResult(res);}

  DataFrame() = default;

  template <typename ResType>
    RowType readRow(ResType res) {
    return getTupleValue<ResType, Args...>(res);
  }

  void insert_from_tuple(const std::tuple<Args...>& tup, int_<sizeof...(Args)>) {
    return;
  }

  template <size_t index>
    void insert_from_tuple(const std::tuple<Args...>& tup, int_<index>) {
    std::get<index>(_data).push_back(std::get<index>(tup));
    insert_from_tuple(tup, int_<index + 1>());
  }

  template <typename ResType>
    void loadResult(ResType res) {
    while(res->next()) {
      ++_nrow;
      insert_from_tuple(readRow(res), int_<0>());
    }
  }

  uint nrow() {return  _nrow;}

  static constexpr uint col() {return (uint) std::tuple_size<RowType>::value;}

  template <size_t j>
    const ctype<j>& column() { return std::get<j>(_data);}

  template <size_t j>
    atype<j> element(uint i) {
    return column<j>()[i];
  }

  template<size_t headIndex, typename Last>
    std::tuple<Last> extractRow(uint i) {
    return std::make_tuple(element<headIndex>(i));
  }

  template <size_t headIndex, typename First, typename... Rest>
    std::tuple<First, Rest...> extractRow(uint i) {
    atype<headIndex> x = element<headIndex>(i);
    return std::tuple_cat(std::make_tuple(x), extractRow<headIndex + 1, Rest...>(i));
  }
  RowType operator[](uint i) {
    return extractRow<0, Args...>(i);
  }

private:
  typename VectorizedTuple<Args...>::type _data;
  uint _nrow = 0;
};
