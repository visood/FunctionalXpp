#include<iostream>
#include<memory>
#include<stdexcept>

namespace Persistent{ namespace Maybe{

template<typename ElemType>
class Maybe
{
public:
	Maybe() = default;
	Maybe(const ElemType& x) :
		_elem(std::make_unique<ElemType>())
	{
		*_elem = x;
	}

	const ElemType& get() const
	{
		if (not _elem) throw std::out_of_range("Maybe is None");

		return *_elem;
	}
	bool containsElem() const {return _elem ? true : false;}

private:
	std::unique_ptr<ElemType> _elem;
};

template<typename ElemType>
bool hasNone(const Maybe<ElemType>& mb)
{
	return not mb.containsElem();
}

template<typename ElemType>
bool hasSome(const Maybe<ElemType>& mb)
{
	return mb.containsElem();
}

template<typename ElemType>
Maybe<ElemType> Some(const ElemType& x)
{
	return Maybe<ElemType>(x);
}

template<typename ElemType>
Maybe<ElemType> None;
}}
