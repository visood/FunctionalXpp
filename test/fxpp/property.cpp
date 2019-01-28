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
    typename ptree_cons< Word, Word, Word >::type;
  Name vcs{pleaf_first, pleaf_middle, pleaf_last};

  CHECK(
    vcs.head.key == "first");
  CHECK(
    vcs.head.value == "Vishal");
  CHECK(
    vcs.tail.head.key == "middle");
  CHECK(
    vcs.tail.head.value == "Chandra");
  CHECK(
    vcs.tail.tail.head.key == "last");
  CHECK(
    vcs.tail.tail.head.value == "Sood");

  using House  = PLeaf<int>;
  using Street = PLeaf<char const*>;
  using City   = PLeaf<char const*>;

  House  my_house{"house", 13};
  Street my_street{"street", "Parontyuan wali gali."};
  City   my_city{"city", "Purani Dilli"};

  CHECK(
    my_house.key == "house");
  CHECK(
    my_house.value == 13);
  CHECK(
    my_street.key == "street");
  CHECK(
    my_street.value == "Parontyuan wali gali.");
  CHECK(
    my_city.key == "city");
  CHECK(
    my_city.value == "Purani Dilli");


  using Address=
    typename ptree_cons<House, Street, City>::type;

  Address my_address{my_house, my_street, my_city};

  using Person=
    typename ptree_cons< PNode<Name>, PNode<Address> >::type;

  Person me{{"name", vcs}, {"address", my_address}};

  CHECK(
    me.head.key == "name");
  CHECK(
    me.head.value.head.key == "first");
  CHECK(
    me.head.value.head.value == "Vishal");
  CHECK(
    me.tail.head.key == "address");

  const auto check_first_name=
    get_value(pleaf_first, "first");
      
  REQUIRE(
    not check_first_name.empty);
  CHECK(
    check_first_name.get() == "Vishal");
}
