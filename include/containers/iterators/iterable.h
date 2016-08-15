/*
	what defines an iterable type?

	Vishal Sood
*/

template<typename ValueType>
class Iterable
{
public:
	virtual std::pair<ValueType, Iterator&> next() = 0;
	virtual bool done() = 0;

}


template<
	template<typename...> typename Iterable,
	typename ValueType
>
class Iterator
{
public :
	using self_type = Iterator;
	using self_reference = Iterator&;
	using iterator_category = std::forward_iterator_tag;
	using value_type = ValueType;
	using difference_type = uint32_t;
	using pointer = value_type const*;
	using reference = value_type const&;

	Iterator(const Iterable<ValueType>& C) :
		_container(&C)
	{}

	self_reference operator++()
	{
		if (not _container->done(*this)) {
			const auto nitr = _container->next(*this);
			_value = nitr.first;
		}
		return *this;
	}
	value_type operator*() const { return _value;}
	bool operator!=(const Iterator& that) const
	{

	}

protected :
	const Iterable<value_type>* const _container;
	value_type _value;

};
