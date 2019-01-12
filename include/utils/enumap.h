/*
	enumerations as maps
*/
#include<util.h>

template<typename EnumType>
struct TermMap
{
	char const* term;
	EnumType value;
};
constexpr char const* TermMapENDMARKER = "end-term";

template<typename EnumType>
constexpr TermMap<EnumType> const* termMap();

template<typename EnumType>
constexpr EnumType getValue(
	char const* term,
	TermMap<EnumType> const* entry = termMap<EnumType>()
){return(
		(util::areEqualStrings(entry->term, TermMapENDMARKER) or
		 util::areEqualStrings(entry->term, term))
		? entry->value
		: getValue<EnumType>(term, entry + 1)
	);
}
