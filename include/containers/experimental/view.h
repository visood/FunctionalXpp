#pragma once

#include <functional>
#include <memory>
#include "utils/trait.h"

/*
  We need to improve the implementation of Views.
  In the current implementation, each chained operation creates a vector eagerly.
  Ideally only one vector should be created when the result needs to be collected.

  View is modeled as a list of operations. Each cell (element) of this list must
  be able to process the result of the previous cell. We use the term
  "received_type" for the type of the value received by a ViewCell, and the term
  "forwarded_type" for the type forwarded by a ViewCell to the next ViewCell in
  the chain. In a chain consisting of only mapping and filtering ViewCells, only
  singletons will be propagated along the chain. View chain will take each
  element of the input container and propagate it along the chain, returning a
  result with the same container type.

*/

/*
  In the definition of ViewCell we will need to resolve the propagated type,
  which will be either a Singleton, or a Collection,
  and will be determined by the input type (Singleton or Collection),
  and the transform policy.
  For FilteringTransfomPolicy  and MappingTransformPolicy, Propagated type will
  be the InputType (so Singleton -> Singleton, Collection -> Collection),
  while for FlatMappingTransformPolicy both Singleton and Collection input types
  will become Collection.
  So we first define transforming policies, and then a type method to resolve
  the propagated type
*/

namespace experimental { namespace collection {

template<
	template<typename...> class ContainerType
	,typename InType
	,typename OutType
	>
struct MappingCell
{
	template<typename... Args>
	using container_type = ContainerType<Args...>;

	using in_type  = InType;
	using out_type = OutType;
	using function_type = std::function<out_type(const in_type&)>;

	static container_type<out_type> invoke(const function_type& transformed,
										   const container_type<in_type>& xs)
	{
		container_type<out_type> ys;
		for (const in_type& x: xs) ys.push_back( transformed(x) );
		return ys;
	}

	static std::shared_ptr<out_type> invoke(const function_type& transformed,
											const std::shared_ptr<in_type> xPtr)
	{
		return xPtr? std::make_shared<out_type>( transformed(*xPtr) ) : nullptr;
	}

};

template<
	template<typename...> class ContainerType
	,typename InType
	>
struct FilteringCell
{
	template<typename... Args>
	using container_type = ContainerType<Args...>;

	using in_type  = InType;
	using out_type = InType;
	using function_type = std::function<bool(const in_type&)>;

	static container_type<out_type> invoke(const function_type& transformed,
										   const container_type<in_type>& xs)
	{
		container_type<out_type> ys;
		for (const in_type& x: xs) if (transformed(x)) ys.push_back(x);
		return ys;
	}

	static std::shared_ptr<out_type> invoke(const function_type& transformed,
											const std::shared_ptr<in_type> xPtr)
	{
		return (xPtr and transformed(*xPtr)) ? std::make_shared<out_type>(*xPtr) : nullptr;
	}
};

template<
	template<typename...> class ContainerType
	,typename InType
	,typename OutType
	>
struct FlatMappingCell
{

	template<typename... Args>
	using container_type = ContainerType<Args...>;

	using in_type  = InType;
	using out_type = OutType;

	using function_type = std::function< container_type<out_type>(const in_type&) >;


	static container_type<out_type> invoke(const function_type& transformed,
										   const container_type<in_type>& xs)
	{
		container_type<out_type> ys;
		for (const in_type& x: xs)
			for ( const typename out_type::value_type & y: transformed(x) )
				ys.push_back(y);
		return ys;
	}

	static container_type<out_type> invoke(const function_type& transformed,
										   const std::shared_ptr<in_type> xPtr)
	{
		return xPtr? transformed(*xPtr) : container_type<out_type>();
	}
};


/*
  a type method, with default evaluation
  InputType may be a std::shared_ptr or a std container
*/
template<
	template<typename...> class InputType
	,typename TransformCell >
struct Propagated
{
	template<typename... Args>
	using type = InputType<Args...>;
};

/*
  when the TransformPolicy is FlatMapping,
  propagated type is a container
*/
template<
	template<typename...> class InputType,
	template<typename...> class ContainerType,
	typename InType,
	typename OutType>
struct Propagated< InputType, FlatMappingCell<ContainerType, InType, OutType > >
{
	template<typename... Args>
	using type = ContainerType<Args...>; 
};

template<
	typename HeadCell
	, typename... TailCells >
class ViewChain
{
public:
	using this_type = ViewChain<HeadCell, TailCells...>;
	using prev_type = ViewChain<TailCells...>;

	using original_type = typename prev_type::original_type;

	using elem_type = typename HeadCell::out_type;

	template<typename... Args>
	using container_type = typename prev_type::template container_type<Args...>;

	template<typename... Args>
	using forwarded_type = typename Propagated<
		prev_type::template forwarded_type,
		HeadCell
		>::template type<Args...>;


	//constructors
	template<typename Transformer>
	ViewChain(const ViewChain<TailCells...>& prev,
			  const Transformer& transformer)
		: _previously(std::move(prev)),
		  _transformer(transformer)
	{}

	ViewChain(const this_type& that)
		: ViewChain(that.previously(), that.transformer())
	{}

	ViewChain(this_type&& that) noexcept
		: _previously(std::move(that.previously())),
		  _transformer(std::move(that.transformer()))
	{}

	const ViewChain<TailCells...>& previously() const {return _previously;}

	const typename HeadCell::function_type& transformer() const { return _transformer; }

	const container_type<original_type>& originalElements() const
	{
		return _previously.originalElements();
	}

	forwarded_type<elem_type> processed(const original_type& x) const
	{
		return HeadCell::invoke(_transformer, _previously.processed(x));
	}

	static void appendTo(container_type<elem_type>& ys,
						 const container_type<elem_type>& xs)
	{
		for (const elem_type& x : xs) ys.push_back(x);
	}

	static void appendTo(container_type<elem_type>& ys,
						 const std::shared_ptr<elem_type> xPtr)
	{
		if (xPtr) ys.push_back(*xPtr);
	}

	container_type<elem_type> collect(const container_type<original_type>& xs) const
	{
		container_type<elem_type> ys;
		for (const original_type& x : xs) appendTo(ys, processed(x));
		return ys;
	}

	container_type<elem_type> collect() const
	{
		return collect(originalElements());
	}

	template<typename NextHeadCellType>
	using Cons = ViewChain<NextHeadCellType, HeadCell, TailCells...>;

	template<
		typename Mapper
		,typename mapped_type = typename std::result_of<Mapper&(elem_type)>::type
		,typename MappingCellType = MappingCell< container_type, elem_type, mapped_type> >
	Cons<MappingCellType> map(const Mapper& mapper) const
	{
		return Cons<MappingCellType>(*this, mapper);
	}
	
	template<
		typename Predicate
		,typename FilteringCellType = FilteringCell< container_type, elem_type> >
	Cons<FilteringCellType> filter(const Predicate& pred) const
	{
		return Cons<FilteringCellType>(*this, pred);
	}

	template<
		typename FlatMapper
		,typename CR = typename std::result_of<FlatMapper(elem_type)>::type
		,typename R  = typename CR::value_type
		,typename FlatMappingCellType = FlatMappingCell< container_type, elem_type, R > >
	Cons<FlatMappingCellType> flatMap(const FlatMapper& fmapper) const
	{
		return Cons<FlatMappingCellType>(*this, fmapper);
	}

	std::size_t size() const { return collect().size(); }

private:
	const ViewChain< TailCells...> _previously;
	const typename HeadCell::function_type _transformer;
};


//specialize ViewChain when its empty

template<
	template<typename...> class ContainerType,
	typename ElemType
	>
class NilCell {};

template<
	template<typename...> class ContainerType,
	typename ElemType
	>
class ViewChain< NilCell<ContainerType, ElemType> >
{
public:
	using original_type = ElemType;

	template<typename... Args>
	using container_type = ContainerType<Args...>;

	using elem_type = ElemType;

	template<typename T>
	using forwarded_type = std::shared_ptr<T>;

	const container_type<original_type>& originalElements() const { return _elems; }

	std::size_t size() const { return _elems.size(); }

	std::shared_ptr<elem_type> processed(const original_type& x) const
	{
		return std::make_shared<elem_type>(x);
	}

	ViewChain(const container_type<original_type>& xs) :
		_elems(xs)
	{}

	template<typename NextHeadCellType>
	using Cons = ViewChain<NextHeadCellType, NilCell<ContainerType, ElemType> >;

	template<
		typename Mapper
		,typename mapped_type = typename std::result_of<Mapper&(elem_type)>::type
		,typename MappingCellType = MappingCell< container_type, elem_type, mapped_type> >
	Cons<MappingCellType> map(const Mapper& mapper) const
	{
		return {*this, mapper};
	}
	
	template<
		typename Predicate
		,typename FilteringCellType = FilteringCell< container_type, elem_type> >
	Cons<FilteringCellType> filter(const Predicate& pred) const
	{
		return {*this, pred};
	}

	template<
		typename FlatMapper
		,typename CR = typename std::result_of<FlatMapper(elem_type)>::type
		,typename R  = typename CR::value_type
		,typename FlatMappingCellType = FlatMappingCell< container_type, elem_type, R > >
	Cons<FlatMappingCellType> flatMap(const FlatMapper& fmapper) const
	{
		return {*this, fmapper};
	}


private:
	const container_type<original_type>& _elems;
};

//helper function

template<
	template<typename...> class ContainerType,
	typename ElemType
	>
ViewChain< NilCell<ContainerType, ElemType> > view(const ContainerType<ElemType>& xs)
{
	return ViewChain< NilCell<ContainerType, ElemType> >(xs);
}

}/*namespace collection */ } /*namespace experimental */
