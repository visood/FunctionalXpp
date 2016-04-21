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

