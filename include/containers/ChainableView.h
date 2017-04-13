
/*
  a trait that specifies a chainable view
  no constructors here
*/

template<typename P, typename M> class MapHeadedView;
template<typename T> class FilterHeadedView;
template<typename P, typename M>  class FlatMapHeadedView;

template<
	typename PrevViewType,
	typename ConcreteView,
	typename MappedType
	>
class ChainableView
{
public:
	size_t size() const { return self()->collect().size(); }

	template<
		typename Mapper,
		typename R  = typename std::result_of<Mapper&(MappedType)>::type
		>
	MapHeadedView< ConcreteView, R > map(const Mapper& nextMapper) const
	{
		return MapHeadedView< ConcreteView, R >(*self(), nextMapper);
	}

	template<
		typename Predicate
		>
	FilterHeadedView< ConcreteView > filter(const Predicate& pred) const
	{
		return FilterHeadedView<ConcreteView>(*self(), pred);
	}

	template<
		typename FlatMapper,
		typename CR = typename std::result_of<FlatMapper&(MappedType)>::type,
		typename R  = typename CR::value_type
		>
	FlatMapHeadedView< ConcreteView, R> flatMap(const FlatMapper& flatMapper) const
	{
		return FlatMapHeadedView<ConcreteView, R>(*self(), flatMapper);
	}
private :
	inline const ConcreteView* self() const
	{
		return static_cast<const ConcreteView*>(this);
	}
};


template<
	typename PrevViewType,
	typename MappedType
	>
class MapHeadedView : public ChainableView<
	PrevViewType,
	MapHeadedView<PrevViewType, MappedType>,
	MappedType
	>
{
public:
	using prev_type   = PrevViewType;
	using this_type   = MapHeadedView<PrevViewType, MappedType>;
	using orig_type   = typename prev_type::orig_type;
	using elem_type   = typename prev_type::mapped_type;
	using mapped_type = MappedType;

	using function  = std::function<MappedType(const elem_type&)>;

	template<typename... args>
	using container = typename prev_type::template container<args...>;

	template<typename Transformer>
	MapHeadedView(const PrevViewType& prev, const Transformer& transform) :
		_previously(std::move(prev)),
		_mapped(transform)
	{}

	MapHeadedView(const this_type& that) :
		MapHeadedView(that.previously(), that.transformer())
	{}

	MapHeadedView(this_type&& that) noexcept :
		_previously(std::move(that.previously())),
		_mapped(std::move(that.transformer()))
	{}

	const PrevViewType& previously() const {return _previously;}

	const function& transformer() const {return _mapped;}

	container<mapped_type> collect() const
	{
		container<mapped_type> ys;
		for (const elem_type& x : _previously.collect())
			ys.push_back( _mapped(x) );
		return ys;
	}

private:
	const PrevViewType _previously;
	const function     _mapped;
};


template<typename PrevViewType>
class FilterHeadedView : public ChainableView<
	PrevViewType,
	FilterHeadedView<PrevViewType>,
	typename PrevViewType::mapped_type
	>
	
{
public :
	using prev_type   = PrevViewType;
	using this_type   = FilterHeadedView<PrevViewType>;
	using orig_type   = typename prev_type::orig_type;
	using elem_type   = typename prev_type::mapped_type;
	using mapped_type = elem_type;

	template<typename... args>
	using container = typename prev_type::template container<args...>;

	using function = std::function<bool(const elem_type&)>;

	template<typename Predicate>
	FilterHeadedView(const PrevViewType& prev, const Predicate& pred) :
		_previously(std::move(prev)),
		_predicate(pred)
	{}

	FilterHeadedView(const this_type& that) :
		FilterHeadedView(that.previously(), that.predicate())
	{}

	FilterHeadedView(this_type&& that) noexcept :
		_previously(std::move(that.previously())),
		_predicate(std::move(that.predicate()))
	{}

	const PrevViewType& previously() const {return _previously;}
	const function& predicate() const {return _predicate;}

	container<elem_type> collect() const
	{
		container<elem_type> ys;
		for (const elem_type& x : _previously.collect()) {
			if (_predicate(x)) ys.push_back(x);
		}
		return ys;
	}
	
private:
	const PrevViewType _previously;
	const function     _predicate;
};

template<
	typename PrevViewType,
	typename MappedType
	>
class FlatMapHeadedView : public ChainableView<
	PrevViewType,
	FlatMapHeadedView<PrevViewType, MappedType>,
	MappedType
	>
{
public:
	
	using prev_type   = PrevViewType;
	using this_type   = FlatMapHeadedView<PrevViewType, MappedType>;
	using orig_type   = typename prev_type::orig_type;
	using elem_type   = typename prev_type::mapped_type;
	using mapped_type = MappedType;

	template<typename... args>
	using container = typename prev_type::template container<args...>;

	using function  = std::function<container<mapped_type>(const elem_type&)>;

	template<typename Transformer>
	FlatMapHeadedView(const PrevViewType& prev, const Transformer& transform) :
		_previously(std::move(prev)),
		_mapped(transform)
	{}

	FlatMapHeadedView(const this_type& that) :
		FlatMapHeadedView(that.previously(), that.transformer())
	{}

	FlatMapHeadedView(this_type&& that) noexcept :
		_previously(std::move(that.previously())),
		_mapped(std::move(that.transformer()))
	{}

	const PrevViewType& previously() const {return _previously;}

	const function& transformer() const {return _mapped;}

	container<mapped_type> collect() const
	{
		container<mapped_type> ys;
		for (const elem_type& x : _previously.collect()) {
			for (const mapped_type& y : _mapped(x))
				ys.push_back(y);
		}
		return ys;
	}

private:
	const PrevViewType _previously;
	const function     _mapped;
};

template<
	template<typename...> class ContainerType,
	typename ElemType
	>
class TransparentView : public ChainableView<
	TransparentView<ContainerType, ElemType>,
	TransparentView<ContainerType, ElemType>,
	ElemType
	>
{
public:
	using orig_type   = ElemType;
	using elem_type   = ElemType;
	using mapped_type = ElemType;

	using this_type   = TransparentView<ContainerType, ElemType>;
	template<typename ...args>
	using container = ContainerType<args...>;

	TransparentView() = default;

	TransparentView(const container<elem_type>& xs) :
		_elements(xs)
	{}

	TransparentView(const this_type& that) :
		TransparentView(that.collect())
	{}

	TransparentView(this_type&& that) noexcept :
		_elements(that.collect())
	{}

	const container<elem_type>& collect() const { return _elements; }

public:
	const container<elem_type>& _elements;
};



template<
	template<typename...> class ContainerType,
	typename elem_type,
	typename ViewType =  TransparentView<ContainerType, elem_type>
	>
ViewType view(const ContainerType<elem_type>& xs)
{
	return ViewType(xs);
}
