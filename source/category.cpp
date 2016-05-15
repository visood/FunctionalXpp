#include "category.h"
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

  //auto factorial = Y(fact);
  //std::cout << (Y(fact))(4) << std::endl;
  std::cout << lambda_factorial(4) << std::endl;
  return 0;
}
