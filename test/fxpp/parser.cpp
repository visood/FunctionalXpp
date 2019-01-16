/*
  Test the parser.
  Vishal Sood
  20190116
*/

#include "catch.hpp"
#include "fxpp/parser.hpp"

using namespace fxpp;
using namespace fxpp::expression;

TEST_CASE(
	"Functional parser basics",
	"[ParserBasicSyntax] [FunctionalParserBasics]"
){
  const String hello_world(
    "hello world, bounjour tous monde, and namastey jee.");
  const auto first_char=
    item(hello_world);
	REQUIRE(
		not first_char.empty);
	CHECK(
		first_char.value == 'h');
	const auto hello_world_y=
    yield(String("hello"))("world");
  REQUIRE(
    not hello_world_y.empty);
  CHECK(
    hello_world_y.value == "hello");
	CHECK(
		hello_world_y.out == "world");
	const auto hi_item=
    item >>= [=] (const char c){
      return yield(
        String("hi") + c) >>= [=] (const String& s){
          return yield(
            String("hi") + s);};};
	const auto hi_result=
    hi_item(hello_world);
	REQUIRE(
		not hi_result.empty);
	const auto hihi_item=
    item >>= [=] (const char c){
      return yield(
        String("hi") + c) >>= [=] (const String& s){
          return yield(
            String("hi") + s);};};
	const auto hihi_result=
    hihi_item(
      hello_world);
	REQUIRE(
		not hihi_result.empty);
	CHECK(
		hihi_result.value == "hihih");
	const auto three_items=
		item >>= [=] (const char c1){
      return
        item >>= [=] (const char c2){
          return
            item >>= [=] (const char c3){
              return
                yield(String(1, c3) + c2 + c1);};};};
                  
	const auto three_result=
    three_items(hello_world);
	REQUIRE(
		not three_result.empty);
	CHECK(
		three_result.value == "leh");

  const auto fourth_item=
    three_items >> item;
  const auto fourth_result=
    parse(
      fourth_item,
      hello_world);
  REQUIRE(
    not fourth_result.empty);
  CHECK(
    fourth_result.value == 'l');

  const auto hello_fourth=
    three_items >> item >>= [=] (const char c) {
      return yield(
        String("hello") + c);};
  const auto hello_fourth_result=
    parse(
      hello_fourth,
      hello_world);
  REQUIRE(
    not hello_fourth_result.empty);
  CHECK(
    hello_fourth_result.value == "hellol");
} 
