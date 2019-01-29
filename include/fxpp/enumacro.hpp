/*
  Defines a macro to auto-generate enums
  20190129
  Vishal Sood
 */

#define ENUMVAL(n, v, d) v,
#define ENUMCASEOUT(n, v, d) case n::v : ostr << d; break;
#define ENUMCLASS(enumName, enumEntries) enum class enumName { \
  enumEntries(enumName, ENUMVAL) \
  NA                             \
}

#define ENUMOUT(enumName, enumEntries) \
  inline std::ostream& operator<<(std::ostream& ostr, const enumName x) \
  {                                                                     \
    switch(x) {                                                           \
      enumEntries(enumName, ENUMCASEOUT)                                \
    default: ostr << "unknown"; break;                                  \
    }                                                                   \
    return ostr;                                                        \
  }                                                                     \

#define ENUM(enumName, enumEntries)             \
  ENUMCLASS(enumName, enumEntries);             \
  ENUMOUT(enumName, enumEntries)
