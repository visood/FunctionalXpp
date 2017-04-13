/*
	Views, lazy container views.
	A view should accumulate operations on the elements of a container, map,
	filter, and flatMap.
	A view applied to a container should be an iterable object.

	To process a container element we will need either a ElementMapper,
	or a ElementFilterer
*/


#include <functional>
//#include <boost/optional.hpp>
#include <experimental/optional>

/*
  a container View is specified by a method that processes elements
*/

//using namespace boost;
using namespace std::experimental;

template< typename in_type, typename out_type >
struct ElemMapper
{
	using function = std::function<out_type(const in_type&)>;

	static optional<out_type> process(const function& mapped,
									  const optional<in_type>& xopt)
	{
		if (xopt) return mapped(xopt.value());
		return {};
	}
};

//specialize element handler for a filter
template< typename in_type >
struct ElemFilter
{
	using function = std::function<bool(const in_type&)>;
		
	static optional<in_type> process(const function& pred,
									 const optional<in_type>& xopt)
	{
		if (xopt and pred(xopt.value())) return xopt;
		return {};
	}
};

template<
	typename in_type,
	template<typename...> class out_container,
	typename out_elem_type
	>
struct ElemFlatMapper
{
	using function = std::function< out_container<out_elem_type> >(const in_type&)>;

	
};

template<
	typename PrevViewType,
	typename ElemHandlingPolicy,
	typename MappedType
	>
class View
{
public:
	using prev_type   = PrevViewType;
	using orig_type   = typename prev_type::orig_type;
	using elem_type   = typename prev_type::mapped_type;
	using mapped_type = MappedType;

	using elem_handler = ElemHandlingPolicy;

	template<typename... args>
	using container = typename prev_type::template container<args...>;

	using this_type   = View<
		prev_type,
		ElemHandlingPolicy,
		mapped_type
		>;

	template<typename out_type>
	using NextMapHeadedView = View<
		this_type,
		ElemMapper< mapped_type, out_type >,
		out_type
		>;

	using NextFilterHeadedView = View<
		this_type,
		ElemFilter< mapped_type>,
		mapped_type
		>;

	template<typename out_type>
	using NextFlatMapHeadedView = View<
		this_type,
		ElemFlatMapper< mapped_type, container<out_type> >,
		out_type
		>;

	template<typename Processor>
	View(const PrevViewType& prev, const Processor& processor) :
		_previously(std::move(prev)),
		_processor(processor)
	{}

	View(const this_type& that) :
		View(that.previously(), that.processor())
	{}

	View(this_type&& that) noexcept :
		_previously(std::move(that.previously())),
		_processor(std::move(that.processor()))
	{}

	const PrevViewType& previously() { return _previously; }

	const typename elem_handler::function& processor() { return _processor; }

	const container<orig_type>& originalElements() const
	{
		return _previously.originalElements();
		
	}

	size_t size() const { return _previously.size(); }

	optional<mapped_type> process(const orig_type& x) const
	{
		return elem_handler::process(_processor, _previously.process(x));
	}

	container<elem_type> collect() const
	{
		container<elem_type> ys;
		for (const orig_type& x : originalElements()) {
			const auto yOpt = elem_handler::process(_processor,
													_previously.process(x));
			if (yOpt) ys.push_back(yOpt.value());
		}
		return ys;
	}

	template<
		typename Mapper,
		typename R = typename std::result_of<Mapper&(mapped_type)>::type
		>
	NextMapHeadedView<R> map(const Mapper& nextMapper) const
	{
		return NextMapHeadedView<R>(*this, nextMapper);
	}

	template<
		typename Predicate
		>
	NextFilterHeadedView filter(const Predicate& pred) const
	{
		return NextFilterHeadedView(*this, pred);
	}

private:
	const PrevViewType& _previously;
	const typename elem_handler::function _processor;
};

template<
	template<typename...> class ContainerType,
	typename ElemType
	>
class NillView
{
public:
	using orig_type   = ElemType;
	using elem_type   = ElemType;
	using mapped_type = ElemType;

	template<typename ...args>
	using container = ContainerType<args...>;

	using this_type = NillView<ContainerType, ElemType>;

	template<typename out_type>
	using MapHeadedView = View<
		this_type,
		ElemMapper< mapped_type, out_type >,
		out_type
		>;

	using FilterHeadedView = View<
		this_type,
		ElemFilter< mapped_type>,
		mapped_type
		>;

	NillView(const container<elem_type>& xs) :
		_elems(xs)
	{}

	optional<elem_type> process(const elem_type& x) const { return x; }

	const container<elem_type>& originalElements() const { return _elems; }

	size_t size() const { return _elems.size(); }

	template<
		typename Mapper,
		typename R = typename std::result_of<Mapper&(mapped_type)>::type
		>
	MapHeadedView<R> map(const Mapper& mapper) const
	{
		return MapHeadedView<R>(*this, mapper);
	}

	template<
		typename Predicate
		>
	FilterHeadedView filter(const Predicate& pred) const
	{
		return FilterHeadedView(*this, pred);
	}

private:
	const container<elem_type>& _elems;
};


//helpers
template<
	template<typename...> class C,
	typename T
	>
NillView<C, T> view(const C<T>& xs) { return NillView<C, T>(xs); }

