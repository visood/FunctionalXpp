/*
  Test nested-properties

 */

#include "catch.hpp"
#include "fxpp/property.hpp"

using namespace fxpp;
using namespace fxpp::property;


TEST_CASE(
  "Nested Property basics",
  "[PropertyBasicSyntax] [PropertyBasics]")
{
  using Word = PLeaf<char const*>;
  Word pleaf_first{"first", "Vishal"};
  CHECK(
    pleaf_first.key == "first");
  CHECK(
    pleaf_first.value == "Vishal");
  CHECK(
    PLeaf<int>::role == "leaf");
  CHECK(
    pleaf_first.role == "leaf");

  Word pleaf_middle{"middle", "Chandra"};
  CHECK(
    pleaf_middle.key == "middle");
  CHECK(
    pleaf_middle.value == "Chandra");

  Word pleaf_last{"last", "Sood"};
  CHECK(
    pleaf_last.key == "last");
  CHECK(
    pleaf_last.value == "Sood");

  using Name=
    typename ptree_cons< Word, Word, Word, PTree<> >::type;
  Name vcs{pleaf_first, pleaf_middle, pleaf_last};
}
