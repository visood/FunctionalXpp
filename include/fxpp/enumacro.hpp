/*
  Defines a macro to auto-generate enums
  20190129
  Vishal Sood
 */

#define ENUMVAL(n, v, d) v,
#define ENUMCASEOUT(n, v, d) case n::v : ostr << d; break;
#define ENUMCLASS(enum_name, ENUMENTRIES) enum class enum_name { \
  ENUMENTRIES(enum_name, ENUMVAL) \
  NA                             \
}

#define ENUMOUT(enum_name, ENUMENTRIES) \
  inline std::ostream& operator<<(std::ostream& ostr, const enum_name x) \
  {                                                                     \
    switch(x) {                                                           \
      ENUMENTRIES(enum_name, ENUMCASEOUT)                                \
    default: ostr << "unknown"; break;                                  \
    }                                                                   \
    return ostr;                                                        \
  }                                                                     \

#define ENUM(enum_name, ENUMENTRIES)             \
  ENUMCLASS(enum_name, ENUMENTRIES);             \
  ENUMOUT(enum_name, ENUMENTRIES)

#include <string>
#include <iostream>

template<
  typename EnumClass>
std::string description(
  const EnumClass ec)
{ std::ostringstream os; os << ec;
  return os.str();}
