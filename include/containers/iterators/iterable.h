/*
	what defines an iterable type?

	Vishal Sood
*/

#if 0
#include <iterator>

#define SELF(CONCRETE_DERIVED) = static_cast<const CONCRETE_DERIVED&>(*this)

//for cleaner syntax
template<typename Iter>
using Category          = typename std::iterator_traits<Iter>::iterator_category;

template<typename Iter>
using Difference_type   = typename std::iterator_traits<Iter>::difference_type;

template<typename Iter>
using Value_type        = typename std::iterator_traits<Iter>::value_type;


template<typename Iter>
using Item_type         = typename std::iterator_traits<Iter>::item_type;

template<typename Iter>
using Reference         = typename std::iterator_traits<Iter>::reference;

template<typename Iter>
using Pointer           = typename std::iterator_traits<Iter>::pointer;


#if 0
template<typename Derived>
class iterable
{
public:
    using iterable_type     = Derived;
    using item_type         = typename iterable_type::item_type;
	using value_type        = typename iterable_type::value_type;
    using index_type        = typename iterable_type::state_type;
	using pointer           = typename iterable_type::pointer;
    using reference         = typename iterable_type::reference;
    using const_reference   = typename iterable_type::const_reference;
    using difference_type   = typename iterable_type::difference_type;

	const Derived* self() const {return static_cast<const Derived*>(this);}

	class iterator : public std::iterator<
		Category<iterable_type>,
		Value_type<iterable_type>,
		Difference_type<iterable_type>,
		Pointer<iterable_type>,
		Reference<iterable_type>
	>
	{
	public:
		iterator(const Derived* cp) :
			_cPtr(cp)
		{}
		iterator(const iterator& it) :
			_cPtr(it.container())
		{}
		~iterator();

		iterator& operator=(const iterator&) {return *this;}
		iterator operator=(const iterator*) const
		{
			return iterator(_cPtr);
		}

		const Derived* container() const {return _cPtr;}
		bool operator==(const iterator&) const;
		bool operator!=(const iterator&) const;

		iterator& operator++()
		{
			if (not _cPtr->done(_index) ) {
				const auto nx = _cPtr->next(_index);
				_item = nx.first;
				_index = nx.second;
			}
			return *this;

		}
		//iterator& operator++(const int);

//this could be item_type as well;
		reference operator*() const {return _item;}

			
		pointer operator->() const {return &_item;}

	private:
		const Derived* _cPtr; //pointer to container with data, should it be able to change the data?
		const value_type _item;
		const index_type _index;

	}; /* class iterator */

	iterator begin() const
	{
		return iterator();
	}
}; /* class iterable */


//provide uint32 to iterables
struct UInts
{
	using item_type = uint32_t;
	using index_type = uint32_t;
	using reference = uint32_t&;
	using const_reference = const uint32_t&;
	using pointer = uint32_t*;
	using difference_type = uint32_t;
};

class Naturals : public iterable<Naturals>
{

};
#endif

template<typename Base>
struct Trait
{
	template<typename Derived>
	const static Derived& implementation(const Base& base)
	{
		return static_cast<const Derived&>(base);
	}
};

template<
	typename DerivedIterable,
	typename Predicate
>
class Iterable : public Trait<Iterable>
{
	const DerivedIterable& self = implementation(*this);
public:
    using iterable_type = DerivedIterable;
    using value_type    = typename iterable_type::value_type;
    using index_type    = typename iterable_type::index_type;

	index_type start() const
	{
		return self.start(); //do we need a different name for start in d?
	}
	std::pair<value_type, index_type> next(const index_type& idx) const
	{
		if (self.done(idx)) return {self.invalid_value(), self.end_index};
		auto [value, index] = self.next(idx); //do we need a differene name for next in d?
		while(not self.pass(value)) [value, index] = self.next(index);
		if (self.pass(value)) return n;
		return next(n.second);
	}
	bool pass(const value_type& v) const
	{
		return self.pass(v);
	}
	bool done(const index_type& idx) const
	{
		return self.done(idx); //do we need a different name for done in d?
	}

	template<typename Predicate2>
	Filterable<Predicate2> withFilter(const Predicate2& pred2) const
	{
		return Filterable<Predicate2>(pred2);
	}
}; /* class IterableBase */



template<typename DerivedIterable>
using Iterable =  Iterable<DerivedIterable, Idfunc>;


struct Naturals : public Iterable<Naturals>
{
  using value_type = uint32_t;
	using index_type = uint32_t;

	index_type start() const
	{
		return 0;
	}
	std::pair<value_type, index_type> next(const index_type& idx) const
	{
		return {idx + 1, idx + 1};
	}
	//Naturals are never done
	bool done(const index_type& idx) const {return false;}

	template<typename Predicate>
	Iterable<value_type> withFilter(const Predicate& pred) const
	{
		return filterable()
	}

}; /* struct Naturals */


template<
	template<typename...> typename iterable_type
>
class InputIterator
{
public :
	using self_type = InputIterator;
	using self_reference = self_type&;
	using iterator_category = std::input_iterator_tag;
	using value_type = typename iterable_type::value_type;
	using difference_type = uint32_t;
	using pointer = value_type const*;
	using reference = value_type const&;

	InputIterator(const iterable_type& C) :
		_container(C)
	{}
/////////////////////////////////////////////////////////////
	self_reference operator++()
	{
		const auto nitr = next(_container, *this);
		_value = nitr.first;
		while (
			not done(_container, _value)
			and
			not pass(_container, _value)
		) {
			const auto nitr = next(_container, *this);
			_value = nitr.first;
		}
	
		return *this;
	}
	value_type operator*() const { return _value;}
	bool operator!=(const Iterator& that) const
	{
    
	}

protected :
	const iterable_type& _container;
	value_type _value;
};

#undef SELF

#endif
