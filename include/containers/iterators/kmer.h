#pragma once

/*
  we want to call Kmers with the following syntax
*/
template<
	size_t k,
	typename IterableElements
	>
class Kmers;

/*
  however, we want to deduce the Iterable type
*/

template <
	size_t k,
	template<typename...> class Iterable,
	typename ValueType
	>
class Kmers< k, Iterable<ValueType> >
{

	using BaseItrType = typename Iterable<ValueType>::const_iterator;
public:
	Kmers( const Iterable<ValueType>& xs): _data(xs) {}

	uint size() { return (uint) (_data.size() - k + 1);}

	class iterator : public std::iterator<std::forward_iterator_tag, ValueType>
	{
	public:
		iterator(BaseItrType bitr,
				 BaseItrType xitr,
				 BaseItrType eitr) :
			_bgnItr(bitr),
			_curItr(xitr),
			_endItr(eitr)
		{}

		iterator(BaseItrType bitr,
				 BaseItrType eitr) :
			_bgnItr(bitr),
			_curItr(bitr),
			_endItr(eitr)
		{}

		std::array<ValueType, k> operator*()
		{
			std::array<ValueType, k>  elem;
			std::copy(_curItr, _curItr + k, std::begin(elem));
			return elem;
		}

		const iterator& operator++() {_curItr++; return *this;}

		BaseItrType currentIterator() const { return _curItr;}

		bool equals(const iterator& that) const
		{
			return _curItr == that.currentIterator();
		}

		bool operator==(const iterator& that) const
		{
			return _curItr == that.currentIterator();
		}

		bool operator!=(iterator& that) const
		{
			return _curItr != that.currentIterator();
		}

		const BaseItrType& start() { _curItr = _bgnItr; return _bgnItr;}

		BaseItrType& next() { _curItr++; return _curItr;}

		bool done() const { return _curItr < _endItr;}

	private:
		const BaseItrType _bgnItr;
		BaseItrType _curItr;
		const BaseItrType _endItr;
		//const uint _k;
	};

	iterator begin()
	{
		return iterator(std::begin(_data),
						std::begin(_data),
						std::end(_data));
	}
	iterator end()
	{
		return iterator(std::begin(_data),
						std::end(_data) - k + 1,
						std::end(_data) - k + 1);
	}

private:
	const Iterable<ValueType> _data;
};

template < typename Iterator, typename VoidOpr >
void for_each(Iterator& itr, const VoidOpr& func) {
	while (not itr.done() ) {
		func(*itr);
		++itr;
	}
}
