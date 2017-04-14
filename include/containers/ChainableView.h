
/*
  a trait that specifies a chainable view
  no constructors here
*/

namespace view {

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
	//size_t size() const { return self()->collect().size(); }

	size_t size() const
	{
		if (not self()->hasBeenCollected())
			self()->collect();
		return self()->elements().size();
	}

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

	MapHeadedView(const PrevViewType& prev,
				  const function& transform,
				  const container<mapped_type>& elems, 
				  const bool wasCollected) :
		_previously(prev),
		_mapped(transform),
		_elements(elems),
		_hasBeenCollected(wasCollected)
	{}

	template<typename Transformer>
	MapHeadedView(const PrevViewType& prev,
				  const Transformer& transform) : 
		_previously(std::move(prev)),
		_mapped(transform),
		_elements({}),
		_hasBeenCollected(false)
	{}

	MapHeadedView(const this_type& that) :
		MapHeadedView(that.previously(),
					  that.transformer(),
					  that.elements(),
					  that.hasBeenCollected())
	{}

	MapHeadedView(this_type&& that) noexcept :
		_previously(std::move(that.previously())),
		_mapped(std::move(that.transformer())),
		_elements(std::move(that.elements())),
		_hasBeenCollected(std::move(that.hasBeenCollected()))
	{}

    const PrevViewType& previously() const         {return _previously;      }
    const function& transformer() const            {return _mapped;          }
    const container<mapped_type>& elements() const {return _elements;        }
    bool hasBeenCollected() const                  {return _hasBeenCollected;}

	const container<mapped_type> collect() const
	{
		if (not _hasBeenCollected) {
			for (const elem_type& x : _previously.collect()) {
				_elements.push_back( _mapped(x) );
			}
			_hasBeenCollected = true;
		}
		return _elements;
	}

private:
	const   PrevViewType           _previously;
	const   function               _mapped;
	mutable container<mapped_type> _elements;
	mutable bool                   _hasBeenCollected = false;
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
	FilterHeadedView(const PrevViewType& prev,
					 const Predicate& pred,
					 const container<elem_type>& elems,
					 const bool wasCollected) :
		_previously(std::move(prev)),
		_predicate(pred),
		_elements(std::move(elems)),
		_hasBeenCollected(wasCollected)
	{}

	template<typename Predicate>
	FilterHeadedView(const PrevViewType& prev,
					 const Predicate& pred) :
		_previously(std::move(prev)),
		_predicate(pred),
		_elements({}),
		_hasBeenCollected(false)
	{}

	FilterHeadedView(const this_type& that) :
		FilterHeadedView(that.previously(),
						 that.predicate(),
						 that.elements(),
						 that.hasBeenCollected())
	{}

	FilterHeadedView(this_type&& that) noexcept :
		_previously(std::move(that.previously())),
		_predicate(std::move(that.predicate())),
		_elements(std::move(that.elements())),
		_hasBeenCollected(std::move(that.hasBeenCollected()))
	{}

	const PrevViewType& previously() const {return _previously;}
	const function& predicate() const {return _predicate;}
	const container<mapped_type>& elements() const {return _elements;}
	bool hasBeenCollected() const {return _hasBeenCollected;}

	const container<elem_type> collect() const
	{
		if (not _hasBeenCollected) {
			for (const elem_type& x : _previously.collect()) {
				if (_predicate(x)) _elements.push_back(x);
			}
			_hasBeenCollected = true;
		}
		return _elements;
	}

private:
	const   PrevViewType         _previously;
	const   function             _predicate;
	mutable container<elem_type> _elements;
	mutable bool                 _hasBeenCollected = true;
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

	FlatMapHeadedView(const PrevViewType& prev,
					  const function& transform,
					  const container<mapped_type>& elems,
					  const bool wasCollected) :
		_previously(prev),
		_mapped(transform),
		_elements(elems),
		_hasBeenCollected(wasCollected)
	{}

	template<typename Transformer>
	FlatMapHeadedView(const PrevViewType& prev,
					  const Transformer& transform) :
		_previously(std::move(prev)),
		_mapped(transform),
		_elements({}),
		_hasBeenCollected(false)
	{}

	FlatMapHeadedView(const this_type& that) :
		FlatMapHeadedView(that.previously(),
						  that.transformer(),
						  that.elements(),
						  that.hasBeenCollected())
	{}

	FlatMapHeadedView(this_type&& that) noexcept :
		_previously(std::move(that.previously())),
		_mapped(std::move(that.transformer())),
		_elements(std::move(that.elements())),
		_hasBeenCollected(that.hasBeenCollected())
	{}

    const PrevViewType& previously() const         {return _previously;      }
    const function& transformer() const            {return _mapped;          }
    const container<mapped_type>& elements() const {return _elements;        }
    bool hasBeenCollected() const                  {return _hasBeenCollected;}

	container<mapped_type> collect() const
	{
		if (not _hasBeenCollected) {
			for (const elem_type& x : _previously.collect()) {
				for (const mapped_type& y : _mapped(x))
					_elements.push_back(y);
			}
			_hasBeenCollected = true;
		}
		return _elements;
	}

private:
	const PrevViewType             _previously;
	const function                 _mapped;
	mutable container<mapped_type> _elements;
	mutable bool                   _hasBeenCollected;
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

	bool hasBeenCollected() const {return true;}

	const container<elem_type>& elements() const {return _elements;}

	const container<elem_type>& collect() const { return _elements; }

public:
	const container<elem_type>& _elements;
};



template<
	template<typename...> class ContainerType,
	typename elem_type,
	typename ViewType =  TransparentView<ContainerType, elem_type>
	>
ViewType collection(const ContainerType<elem_type>& xs)
{
	return ViewType(xs);
}

namespace monadic {

template<
	template<typename...> class ContainerType,
	typename elem_type,
	typename ViewType = TransparentView<ContainerType, elem_type>,
	typename Mapper,
	typename R = typename std::result_of<Mapper&(elem_type)>::type,
	typename NextViewType = MapHeadedView<ViewType, R>
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
	typename NextViewType = FilterHeadedView<ViewType>
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
	typename NextViewType = FlatMapHeadedView<ViewType, R>
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

} //namespace monadic

} //namespace view
