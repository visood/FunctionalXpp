#include "ChainableView.h"


namespace view { namespace monadic {

template<
	template<typename...> class ContainerType,
	typename elem_type,
	typename ViewType = TransparentView<ContainerType, elem_type>,
	typename Mapper,
	typename R = typename std::result_of<Mapper&(elem_type)>::type,
	typename NextViewType = MapHeadedView<ContainerType, ViewType, R>
	>
ContainerType<R> operator >= (const ContainerType<elem_type>& xs,
							  const Mapper& mapper)
{
	return ViewType(xs).map(mapper).collect();
}

template<
	template<typename...> class ContainerType,
	typename elem_type,
	typename ViewType = TransparentView<ContainerType, elem_type>,
	typename Predicate,
	typename NextViewType = FilterHeadedView<ContainerType, ViewType>
	>
ContainerType<elem_type> operator &= (const ContainerType<elem_type>& xs,
									  const Predicate& pred)
{
	return ViewType(xs).filter(pred).collect();
}

/*
  monadic bind
*/

template<
	template<typename...> class ContainerType,
	typename elem_type,
	typename ViewType = TransparentView<ContainerType, elem_type>,
	typename FlatMapper,
	typename CR = typename std::result_of<FlatMapper&(elem_type)>::type,
	typename R  = typename CR::value_type,
	typename NextViewType = FlatMapHeadedView<ContainerType, ViewType, R>
	>
ContainerType<R> operator >>= (const ContainerType<elem_type>& xs,
							   const FlatMapper& fmapper)
{
	return ViewType(xs).flatMap(fmapper).collect();
}

template<
	template<typename...> class ContainerType,
	typename T,
	typename S
	>
ContainerType<S>& operator >> (const ContainerType<T>& xs,
							   const ContainerType<S>& ys)
{
	(void)(xs);
	return ys;
}

template<
	template<typename...> class ContainerType,
	typename T
	>
ContainerType<T> yield(const T& x)
{
	return {x};
}

template<
	template<typename...> class ContainerType,
	typename T
	>
ContainerType<T> failure(const std::string& msg)
{
	(void)(msg); //msg may be used in an error
	return {};
}

} /*namespace monadic*/ } /*namespace view*/
