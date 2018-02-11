/*
	functional utilities
*/
#pragma once

#include <type_traits>
#include <memory>
#include <algorithm>
#include <set>


namespace fpp {

template<
	template<typename...> class Container,
	typename Element
	>
void puts(const Container<Element>& xs)
{
	std::for_each(begin(xs), end(xs),
				  [](const Element& x) { std::cout << x << ", "; });
	std::cout << std::endl;
}

template<
	template<typename...> class Container,
	typename T1,
	typename T2,
	typename R = std::pair<T1, T2>
	>
Container<R> zip(const Container<T1>& xs, const Container<T2>& ys)
{
	const auto l =  xs.size() <= ys.size() ? xs.size() : ys.size();
	Container<R> xys(l);
	std::transform(begin(xs), begin(xs) + l, begin(ys), begin(xys),
				   [](const T1& x, const T2& y)->R { return {x, y};}); 
}
template<
	template<typename...> class C,
	typename T,
	typename Predicate
	>
bool any(const Predicate& p, const C<T>& t)
{
	return std::any_of(begin(t), end(t), p);
}
template<
	template<typename...> class C,
	typename T,
	typename Predicate
	>
bool all(const Predicate& p, const C<T>& t)
{
	return std::all_of(begin(t), end(t), p);
}

template<
	template<typename...> class C,
	typename T
	>
bool operator ==(const C<T>& xs, const C<T>& ys)
{
	return xs.size() == ys.size() and all(
		[](const auto& xy) {return xy.first == xy.second;},  zip(xs, ys)
	);
}

/*
  we can get the max over a container
  what if the container is empty? --- following code will segfault
  we need default values defined somewhere for segfault safe behavior.
*/
template<
	template<typename...> class Container,
	typename Element,
	typename Comparator
	>
Element max(const Comparator& comp,
			const Container<Element>& xs,
			const Element& defmax)
{
	xs.size() == 0 ? defmax : *std::max_element(begin(xs), end(xs), comp);
}
	
template<
	template<typename...> class Container,
	typename Element
	>
Element max(const Container<Element>& xs,
			const Element& defmax)
{
	return xs.size() == 0 ? defmax : *std::max_element(begin(xs), end(xs));
}

/*
  To get max of elements, and if elements is empty the default value, we
  can have a better expression:
*/
template<
	template<typename...> class Container,
	typename Element,
	typename Comparator
	>
Element maxOrElse(const Comparator& comp,
				  const Container<Element>& xs,
				  const Element& defmax)
{
	xs.size() == 0 ? defmax : *std::max_element(begin(xs), end(xs), comp);
}

template<
	template<typename...> class Container,
	typename Element
	>
Element maxOrElse(const Container<Element>& xs,
			const Element& defmax)
{
	return xs.size() == 0 ? defmax : *std::max_element(begin(xs), end(xs));
}

/*
  throw Exception if called without a default max and array size is 0
*/
template<
	template<typename...> class Container,
	typename Element,
	typename Comparator
	>
Element max(const Comparator& comp,
			const Container<Element>& xs)
{
	throwExceptionIf(xs.size() == 0);
	return *std::max_element(begin(xs), end(xs), comp);
}
template<
	template<typename...> class Container,
	typename Element
	>
Element max(const Container<Element>& xs)
{
	throwExceptionIf(xs.size() == 0);
	return *std::max_element(begin(xs), end(xs));
}

/*
  what if the container is empty?
  a safer max operation
*/

template<
	template<typename...> class Container,
	typename Element,
	typename Comparator
	>
Element min(const Comparator& comp,
			const Container<Element>& xs,
			const Element& defmin)
{
	xs.size() == 0 ? defmin : *std::min_element(begin(xs), end(xs), comp);
}
	
template<
	template<typename...> class Container,
	typename Element
	>
Element min(const Container<Element>& xs,
			const Element& defmin)
{
	return xs.size() == 0 ? defmin : *std::min_element(begin(xs), end(xs));
}
/*
  To get min of elements, and if elements is empty the default value, we
  can have a better expression:
*/
template<
	template<typename...> class Container,
	typename Element,
	typename Comparator
	>
Element minOrElse(const Comparator& comp,
				  const Container<Element>& xs,
				  const Element& defmax)
{
	xs.size() == 0 ? defmax : *std::min_element(begin(xs), end(xs), comp);
}

template<
	template<typename...> class Container,
	typename Element
	>
Element minOrElse(const Container<Element>& xs,
				  const Element& defmax)
{
	return xs.size() == 0 ? defmax : *std::min_element(begin(xs), end(xs));
}



/*
  throw an exception if no default provided, and array size is 0
*/

template<
	template<typename...> class Container,
	typename Element,
	typename Comparator
	>
Element min(const Comparator& comp,
			const Container<Element>& xs)
{
	throwExceptionIf(xs.size() == 0);
	return *std::min_element(begin(xs), end(xs), comp);
}
template<
	template<typename...> class Container,
	typename Element
	>
Element min(const Container<Element>& xs)
{
	throwExceptionIf(xs.size() == 0);
	return *std::min_element(begin(xs), end(xs));
}

//specializations of max for a set
template<typename Element, typename... Args>
Element max(const std::set<Element, Args...>& xs, const Element& defmax)
{
	return xs.empty() ? defmax : *xs.rbegin();
}

template<typename Element, typename... Args>
Element maxOrElse(const std::set<Element, Args...>& xs, const Element& defmax)
{
	return xs.empty() ? defmax : *xs.rbegin();
}

//specializations of max for a set
template<typename Element, typename... Args>
Element max(const std::set<Element, Args...>& xs)
{
	throwExceptionIf(xs.empty());
	return *xs.rbegin();
}

//specializations of min for a set
template<typename Element, typename... Args>
Element min(const std::set<Element, Args...>& xs, const Element& defmin)
{
	return xs.empty() ? defmin : *xs.begin();
}

template<typename Element, typename... Args>
Element minOrElse(const std::set<Element, Args...>& xs, const Element& defmin)
{
	return xs.empty() ? defmin : *xs.begin();
}

//specializations of min for a set
template<typename Element, typename... Args>
Element min(const std::set<Element, Args...>& xs)
{
	throwExceptionIf(xs.empty())
	return *xs.begin();
}


template<
	template<typename...> class Container,
	typename Element
	>
bool contains(const Element& v, const Container<Element>& xs)
{
	return any([&v] (const Element& x) {return x == v;}, xs);
}

template<typename T>
bool contains(const T& v, const std::set<T>& xs)
{
	return xs.find(v) != xs.end();
}

inline bool contains(const char c, const String& s)
{
	return s.find(c) != std::string::npos;
}
template<
	template<typename...> class C,
	typename T
	>
C<T> intersection(const std::set<T>& xs, const C<T>& ys)
{
	C<T> zs;
	set_intersection(xs.begin(), xs.end(),
					 ys.begin(), ys.end(),
					 std::back_inserter(zs));
	return zs;
}
//split a string into a container of a given type
template<
	typename T,
	template<typename...> class C
	>
C<T> split(char const* ss, const char sep)
{
	if (ss == nullptr) return {};
	C<T> xs;
	T x;
	for (const String& s : Util::split(ss, sep))
		if (Util::value<T>(x, s.c_str())) xs.push_back(x);
	return xs;
}
template<
	typename T,
	template<typename...> class C
	>
C<T> split(const String& ss, const char sep) {return split<T, C>(ss.c_str(), sep);}

template<typename T>
std::vector<T> split(char const* ss, const char sep)
{
	return split<T, std::vector>(ss, sep);
}
template<typename T>
std::vector<T> split(const String& ss, const char sep)
{
	return split<T, std::vector>(ss, sep);
}

template<
	template<typename... > class C,
	typename T1,
	typename Function,
	typename T2 = typename std::result_of<Function&(T1)>::type
	>
C<T2> map(const Function& f, const C<T1>& xs)
{
	C<T2> ys(xs.size());
	std::transform(begin(xs), end(xs), begin(ys), f);
	return ys;
}

/*
  C++ set does not behave the same way as a list or vector
*/
template<
	typename T1,
	typename Function,
	typename T2 = typename std::result_of<Function&(T1)>::type
	>
std::set<T2> map(const Function& f, const std::set<T1>& xs)
{
	std::set<T2> ys;
	for (const T1& x : xs) ys.insert(f(x));
	return ys;
}

template<
	template<typename... > class C,
	typename T1,
	typename T2,
	typename Function //:: (T2, T1) -> T2
	>
T2 foldl(const T2& y, const Function& f, const C<T1>& xs)
{
	return std::accumulate(begin(xs), end(xs), y, f);
}
template<
	template<typename... > class C,
	typename T1,
	typename T2,
	typename Function //:: (T2, T1) -> T2
	>
T2 sum(const Function& f, const C<T1>& xs)
{
	return foldl(T2(), f, xs);
}

template<
	template<typename... > class C,
	typename T
	>
T sum(const C<T>& xs, const T t0)
{
	return foldl(t0, [] (const T& s, const T& x) {return s + x;}, xs);
}
template<
	template<typename... > class C,
	typename T
	>
T sum(const C<T>& xs) {return sum(xs, T());}


template<
	template<typename... > class C,
	typename T
	>
double mean(const C<T>& xs)
{
	return (double) sum(xs, (T) 0) / (double) xs.size();
}
	

template<
	template<typename... > class C,
	typename T1,
	typename T2,
	typename Function //:: (T2, T1) -> T2
	>
T2 foldr(const C<T1>& xs, const Function& f, const T2& y)
{
	return std::accumulate(begin(xs), end(xs), y, f);
}

/*
  unlike other languages that work with containers, and not iterators,
  C++ does not provide a filter operation,
  Filter :: (a -> Bool) -> Container a -> Container a
  instead erase remove_if idiom is used.
  here we showcase this idiom by writing our own filter.

  to understand the erase remove idiom, remove_if will move all the elements
  satisfying the predicate to the front of the container (keeping the order) --
  the return value is the iterator to one-past the predicate-satisfying elements.
  thus the elements past the returned iterator are invalid -- and we erase them.
  because, the operations change the input container itself, we cannot pass
  a const reference, while a non-const reference will change the input container.
  thus we receive the container xs as a value so that the original container's
  copy is changed.
  also, filter should keep the elements that satisfy the predicate. we have to
  use the negation of the predicate inside remove_if
*/

template<typename Predicate>
auto negation(const Predicate& p)
{
	return [&p] (const auto& x) -> bool {return not p(x);};
}
template<
	template<typename... > class C,
	typename T,
	typename Predicate
	>
C<T> filter(const Predicate& f, C<T> xs)
{
	xs.erase(std::remove_if( xs.begin(), xs.end(), negation(f) ), xs.end());
	return xs;
}

/*
  C++ sets behave differently than lists and vectors
*/
template<
	typename T,
	typename Predicate
	>
std::set<T> filter(const Predicate& f, std::set<T> xs)
{
	std::set<T> ys;
	std::copy_if(xs.begin(), xs.end(), std::inserter(ys, ys.end()), f);
	return ys;
}

/*
  the hardest operation is the flatmap
  Fmap :: (T1 -> Container T2) -> Container T1 -> Container T2
  to understand this, consider we have a function that maps an int to a list of
  ints --- we want to apply this function to each element of a container and
  return the result as a (flattened) list of ints (and not as a list of lists
  of ints as a simple map would do)
*/
template<
	template<typename... > class Container,
	typename T1,
	typename Function,
	typename CT2 = typename std::result_of<Function&(T1)>::type,
	typename T2  = typename CT2::value_type
	>
Container<T2> fmap(const Function& f, const Container<T1>& xs)
{
	Container<T2> zs;
	for (const auto& x : xs) {
		const Container<T2> ys = f(x);
		zs.insert(end(zs), begin(ys), end(ys));
	}
	return zs;
}


/*
	More C++ like expression.
	In C++, or any other oo-lang we are like to express ourselves through methods
	applied to an object: "xs.map(func).filter(pred)". The functions above may
	thus seem a bit un-C++, which we can take care of by providing a view for a
	container. We provide only basic functionality, with eager evaluation for now.
	So it is basically syntactic sugar. However a lazy evaluation and arbitrarily
	long chaining of mapping will make these views quite powerful.

	This is too complicated. Will need to think about chained operations on
	individual elements. 

	Or we can have the class View contain only the operations --- with the
	collect method that receives the container. This can be easily done without
	the facility to fmap (map each element to a list and flatten)

	Operations filter and map do not compute -- obviously.
	We will variadic templates
*/

template<
	typename T1,
	typename Function,
	typename T2 = typename std::result_of< Function&(T1) >::type
	>
struct FilterOrMap
{
	using elem_type = T1;
	using ret_type  = T2;

};

template<typename T>
struct alltrue
{
	bool operator() (const T& ) {return true;}
};

template<typename T>
struct idmap
{
	const T& operator() (const T& x) {return x;}
};

template<typename T>
struct function_traits;

template<typename R, typename T>
struct function_traits<std::function<R(T)>>
{
    using res_type = R;
	using arg_type = T;
};


} /*namespace fpp*/

