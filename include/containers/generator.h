/*
  Author: Vishal Sood
  Date: 20160819

  

*/
#pragma once

#include "util.h"

template<
	typename T,
	typename S
>
class Generator
{
public:
	using value_type = T;
	using state_type = S;
	using self_type = Generator<T,S>;
	using self_reference = Generator<T,S>&;

	virtual Generator<T>& start(); 
	virtual value_type head() const = 0;
	virtual self_reference tail() = 0; //for efficiency, be a reference, and not a const
	virtual Generator<T> tail() const = 0; //returns a value
	virtual void advance(const uint32_t n) = 0;
	virtual bool done() const = 0;
	virtual bool equals(const self_reference that) = 0;
}; /* class Generator */


template<typename DerivedIterable>
class Iterable : public Generator<
	typename DerivedIterable::item_type,
	typename DerivedIterable::state_type
>
{
public:
	using self_type = DerivedIterable;
	using self_reference = self_type&;
	using iterable_type = typename self_type::value_type;
	using item_type = typename self_type::item_type;
	using state_type = typename self_type::state_type;
	using difference_type = typename self_type::difference_type;
	using pointer = item_type const*;
	using reference = item_type const&;

	inline self_type const* self() const
	{
		return static_cast<const self_type*> (this);
	}
	class InputIterator
	{
	public:
		using self_type = InputIterator;
		using self_reference = self_type&;
		using iterator_category = std::input_iterator_tag;
		using container_type = typename Iterable<DerivedIterable>::self_type;
		using item_type = typename container_type::item_type;

		self_reference operator++()
		{
			_container->advance(1);
			return *this;
		}

		const item_type operator*() const { return self()->head();}

		inline bool equals(const self_type& that) const
		{
			return (
				_container.equals(that.container()) and
				_state == that.state()
			);
		}
		bool operator !=(const self_type& that) const
		{
			return not equals(that);
		}

		bool operator ==(const self_type& that) const
		{
			return equals(that);
		}
	private:
		const container_type* const _container;
	}; /* class Iterable::Iterator */
}; /* class Iterable */
	
