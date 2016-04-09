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


using namespace category;

int main()
{ 
  auto f = [] (auto x) { return x*x; };
  auto g = [] (auto x) { return x+2; };
  auto phi = [] (auto x, auto y) { return x+y; };
  auto print = [] (auto x) { std::cout << x << " "; return x; };
  auto list1 = List(3,4,5);
  //auto list2 = fmap(f)(list1);
  //fmap(print)(fmap(f)(list1));
 
  // test if fmap satisfies functor axioms
  auto list2 = fmap(g^f)(list1);
  auto list3 = fmap(g)(fmap(f)(list1));
  auto list4 = list1 > (g^f);
  auto list5 = list1 > f > g;
  list2 > print;
  std::cout << std::endl;
  list3 > print;
  std::cout << std::endl;
  list4 > print;
  std::cout << std::endl;
  list1 > f > g > print;
  std::cout << std::endl;
  std::cout << curry(phi)(3)(4) << std::endl;
  std::cout << bind(unit(3))(f) << std::endl;
  std::cout << (3 >= f >= f) << std::endl;
  3 >= f >= (g^f) >= (g+f) >= print;
  std::cout << std::endl;
  return 0;
}
