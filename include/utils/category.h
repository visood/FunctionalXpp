#include <iostream>

// Can be compiled and tested simply with "g++ -std=c++14 -o category category.cpp && ./category"
// Lists as implemented here can be printed in reverse order, depending on the compiler. This is because
// the order of pack expansion is not defined by the standard. With gcc 5, there is
// still a bug, whereas clang do it the right way.

namespace category
{
// Here the main category is CPP. The objects are C++ types and morphisms are C++ lambdas.
// We will also use the subcategory LIST, containg only list types and morphisms between lists.

// Identity morphism
auto Id = [] (auto x) { return x; };

// Define algebra of real-valued functions
auto Add = [] (auto f, auto g)
{
  return [=] (auto x) { return f(x) + g(x); };
};

auto Substract = [] (auto f, auto g)
{
  return [=] (auto x) { return f(x) - g(x); };
};

auto Multiply = [] (auto f, auto g)
{
  return [=] (auto x) { return f(x) * g(x); };
};

// Compose two morphisms if it is possible
auto Compose = [] (auto f, auto g)
{
  return [=] (auto x) { return g(f(x)); };
};

// Overloads
template <typename F, typename G>
auto operator + (F f, G g)
{
  return Add(f,g);
}

template <typename F, typename G>
auto operator - (F f, G g)
{
  return Substract(f,g);
}

template <typename F, typename G>
auto operator * (F f, G g)
{
  return Multiply(f,g);
}

// Overload compose with ^
template <typename F, typename G>
auto operator ^ (G g, F f)
{
  return Compose(f,g);
}

//recursive lambda factorial
auto lambda_factorial = [] (int n)
{
  auto f = [] (int n, auto lambda) -> int
  {
    return n==0 ? 1 : n*lambda(n-1,lambda);
  };
  return f(n,f);
};

// Factorial "without" recursion
auto fact = [] (auto f)
{
  return [f] (int n) -> int
  {
    return n==0 ? 1 : n*f(n-1);
  };
};

// Some kind of Y combinator
auto Y = [] (auto f)
  {
    return f([f] (int n) -> int { return Y(f)(n); });
  };


// Simple wrapper for a double for currying
auto Double = [] (auto x)
{
  return [=] (auto f) { return f(x); };
};

// Currying
auto curry = [] (auto f)
{
  return [f] (auto x)
  {
    return [=] (auto y) { return f(x,y); };
  };
};

// Wrapper around a parameter pack
// Returns a lambda expecting a lambda that will be mapped to the list
auto List = [] (auto ...elements)
{
  return [=] (auto f) { return f(elements...); };
};

// fmap is a functor (x->y) => (f(x) -> f(y))
// Return a lambda expecting a lambda-list and have the same return type as List
// This is needed in order to chain operations
auto fmap = [] (auto f)
{
  return [f] (auto list)
  {
    return list([f] (auto ...elements) { return List(f(elements)...); });
  };
};

// Overload fmap for haskell-style notations
template <typename F, typename L>
auto operator > (L l, F f)
{
  return fmap(f)(l);
}

// We can have monads by combining unit (return in haskell) and bind
auto unit = [] (auto x)
{
  return [=] () { return x; };
};

auto bind = [] (auto u)
{
  return [=] (auto callback)
  {
    return callback(u());
  };
};

// Overload bind
template <typename F, typename Element>
auto operator >= (Element x, F f)
{
  return bind(unit(x))(f);
}

// generic lambda that returns the size of a parameter pack
auto pack_size = [] (auto ...elements)
{
  return sizeof...(elements);
};

} // end namespace category

