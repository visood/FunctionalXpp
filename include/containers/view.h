/*
	Views, lazy container views.
	A view should accumulate operations on the elements of a container, map,
	filter, and flatMap.
	A view applied to a container should be an iterable object.

	To process a container element we will need either a ElementMapper,
	or a ElementFilterer
*/


#include <functional>

/*
  a container View is specified by a method that processes elements
*/

template<
	typename in_type,
	template<typename...> class container,
	typename out_type
	>
struct MapTransform
{
	using function = std::function<out_type(const in_type&)>;

	static container<out_type> transform(const function& mapped,
										 const container<in_type>& xs)
	{
		container<out_type> ys;
		for (const in_type& x : xs) ys.push_back( mapped(x) );
		return ys;
	}
};

template<
	typename in_type,
	template<typename...> class container
	>
struct FilterTransform
{
	using function = std::function<bool(const in_type&)>;

	static container<in_type> transform(const function& pred,
										const container<in_type>& xs)
	{
		container<in_type> ys;
		for (const in_type& x : xs) {
			if (pred(x)) ys.push_back(x);
		}
		return ys;
	}
};

template<
	typename in_type,
	template<typename...> class container,
	typename out_type
	>
struct FlatMapTransform
{
	using function = std::function< container<out_type>(const in_type&)>;

	static container<out_type> transform(const function& fmapped,
										 const container<in_type>& xs)
	{
		container<out_type> ys;
		for (const in_type& x : xs) {
			for (const out_type& y : fmapped(x)) {
				ys.push_back(y);
			}
		}
		return ys;
	}
};

template<
	typename PrevViewType,
	typename CollectionPolicy,
	typename MappedType
	>
class View
{
public:
	using prev_type   = PrevViewType;
	using orig_type   = typename prev_type::orig_type;
	using elem_type   = typename prev_type::mapped_type;
	using mapped_type = MappedType;

	using collection_policy = CollectionPolicy;

	template<typename... args>
	using container = typename prev_type::template container<args...>;

	using this_type   = View<
		prev_type,
		collection_policy,
		mapped_type
		>;

	template<typename out_type>
	using NextMapHeadedView = View<
		this_type,
		MapTransform< mapped_type, container,  out_type >,
		out_type
		>;

	using NextFilterHeadedView = View<
		this_type,
		FilterTransform< mapped_type, container >,
		mapped_type
		>;

	template<typename out_type>
	using NextFlatMapHeadedView = View<
		this_type,
		FlatMapTransform< mapped_type, container, out_type >,
		out_type
		>;

	template<typename Transformer>
	View(const PrevViewType& prev, const Transformer& transformer) :
		_previously(std::move(prev)),
		_transformer(transformer)
	{}

	View(const this_type& that) :
		View(that.previously(), that.transformer())
	{}

	View(this_type&& that) noexcept :
		_previously(std::move(that.previously())),
		_transformer(std::move(that.transformer()))
	{}

	const PrevViewType& previously() { return _previously; }

	const typename collection_policy::function& transformer() {return _transformer;}

	size_t size() const { return _previously.size(); }

	container<elem_type> collect() const
	{
		return collection_policy::transform(_transformer, _previously.collect());
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

	template<
		typename FlatMapper,
		typename CR = typename std::result_of<FlatMapper&(mapped_type)>::type,
		typename R  = typename CR::value_type
		>
	NextFlatMapHeadedView<R> flatMap(const FlatMapper& flatMapper) const
	{
		return NextFlatMapHeadedView<R>(*this, flatMapper);
	}


private:
	const PrevViewType& _previously;
	const typename collection_policy::function _transformer;
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
		MapTransform< mapped_type, container,  out_type >,
		out_type
		>;

	using FilterHeadedView = View<
		this_type,
		FilterTransform< mapped_type, container >,
		mapped_type
		>;

	template<typename out_type>
	using FlatMapHeadedView = View<
		this_type,
		FlatMapTransform< mapped_type, container, out_type >,
		out_type
		>;

	NillView(const container<elem_type>& xs) :
		_elems(xs)
	{}

	const container<elem_type>& collect() const { return _elems;}

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

	template<
		typename FlatMapper,
		typename CR = typename std::result_of<FlatMapper&(mapped_type)>::type,
		typename R  = typename CR::value_type
		>
	FlatMapHeadedView<R> flatMap(const FlatMapper& flatMapper) const
	{
		return FlatMapHeadedView<R>(*this, flatMapper);
	}

private:
	const container<elem_type>& _elems;
};


//helpers
template<
	template<typename...> class C,
	typename T
	>
NillView<C, T> collectionView(const C<T>& xs) { return NillView<C, T>(xs); }
