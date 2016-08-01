#pragma once

#include <cstdint>
#include <limits>
#include <dirent.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <map>
#include <tuple>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <limits>
#include <tuple>
#include<unordered_map>

using uint = uint32_t;

std::string wordyOneToNine(uint);

std::string wordyInteger(uint);

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
std::string strtup(const Tuple& t, int_<1>, const char delim = ',')
{
    std::stringstream s;
    s << std::get<std::tuple_size<Tuple>::value - 1>(t) << delim;
    std::string st = s.str();
    st.pop_back();
    return st;
}


//for a tuple header we can create a map from the elements to indexes
//this can use either std::tuple,
//or std::array for class Tuple,
//the latter more appropriate because of the same type of elements
//in the container
template <class Tuple, size_t Pos>
std::unordered_map<std::string, uint> tupindexes(
	const Tuple& t,
	int_<Pos>
) {
    auto map1 = tupindexes(t, int_<Pos-1>());
    map1[std::get<Pos - 1>(t)] = Pos - 1;
    return map1;
}

template <class Tuple>
std::unordered_map<std::string, uint> tupindexes(
	const Tuple& t,
	int_<1>
) {
    std::unordered_map<std::string, uint> map;
    map[std::get<0>(t)] = 0;
    return map;
}

template <typename... Args>
std::unordered_map<std::string, uint> tupindexes(Args... names) {
	std::tuple<Args...> tup{names...};
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
    template <std::size_t index>
    using type = typename std::tuple_element<
		index, std::tuple<Args...>
	>::type;
};

template <typename T>
struct _type_{};

template<typename... Args>
struct ArgPack {
	template <std::size_t N>
	using type = typename std::tuple_element<N, std::tuple<Args...> >::type;

	template <class ResType>
	static int getValue(ResType res, uint idx, _type_<int>) {
		return res->getInt(idx + 1);
	}

	template <class ResType>
	static uint getValue(ResType res, uint idx, _type_<uint>) {
		return res->getUInt(idx + 1);
	}

	template <class ResType>
	static double getValue(ResType res, uint idx, _type_<double>) {
		return res->getDouble(idx + 1);
	}

	template <class ResType>
	static std::string getValue(ResType res, uint idx, _type_<std::string>) {
		return res->getString(idx + 1);
	}

	template <class ResType, std::size_t IDX>
    static type<IDX> Element(ResType res, int_<IDX>) {
		return getValue(res, IDX, _type_< type<IDX> >());
	}
};



template<class ResType, typename First, typename... Rest>
static std::tuple<First, Rest...> getTupleValue(ResType res, uint offset = 0) {
	auto x = ArgPack<>::getValue(res, offset, _type_<First>());
	auto tup = getTupleValue<ResType, Rest...>(res, offset + 1);
	return std::tuple_cat( std::make_tuple(x), tup);
}
template<class ResType>
static std::tuple<> getTupleValue(ResType, uint) {
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


template <template <typename...> typename Container,
	template <typename...> typename Mod,
	typename... Args>
struct ParameterModifiedContainer {
    using type = Container < Mod<Args>...>;
};


template <typename... Args>
using VectorizedTuple = ParameterModifiedContainer<std::tuple, std::vector, Args...>;


