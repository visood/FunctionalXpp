/*
	More C++ like expression.
	In C++, or any other oo-lang we are like to express ourselves through methods
	applied to an object: "xs.map(func).filter(pred)". The functions above may
	thus seem a bit un-C++, which we can take care of by providing a view for a
	container. We provide only basic functionality, with eager evaluation for now.
	So it is basically syntactic sugar. However a lazy evaluation and arbitrarily
	long chaining of mapping will make these views quite powerful.

	This is too complicated. Will need to think about chained operations on
	individual elements. 

	Or we can have the class View contain only the operations --- with the
	collect method that receives the container. This can be easily done without
	the facility to fmap (map each element to a list and flatten)

	Operations filter and map do not compute -- obviously.
	We will variadic templates
*/

namespace experimental {

/*
  Lazy views for filter and map operations have been successfully implemented.
  They are lazy because they defer processing of the data till the last moment,
  and use only a single loop over the original elements that are being viewed.

  Incorporating flatMap operations is hard.
  A flatMap's transform function returns a collection unlike map's transform
  that returns singletons. Treating a flatMap just like a map will produce a
  collection of collections. Chaining several flatMaps with such an
  implementation will lead to nested collections. So we have to keep the return
  of a FlatMapHeadedView flat.

  We will return a ViewReturnType 
*/

#if 0
template<typename T>
struct ViewReturnType;

template<typename T>
struct ViewReturnType<T>
{
	const T& elem;
};

template<
	template<typename...> class C,
	typename T
	>
struct ViewReturnType< C<T> >
{
	const C<T>& elem;
};
#endif

//or may be we can use a type method

template<typename... Args>
struct Collection;


template<typename ElemType>
struct Singleton
{
	std::shared_ptr<ElemType> elemPtr;


	//for now use a std::function, generalize to a functor later
	template<
		typename MappedElemType,
		typename FunctionType = std::function<MappedElemType(const ElemType&)>
		>
	Singleton<MappedElemType>
	mapWith(const FunctionType& transform) const
	{
		if (elemPtr) return { std::make_shared<MappedType>(transform(*elemPtr))};
		return {nullptr};
	}

	Singleton<ElemType>
	filterWith(const std::function<bool(const ElemType&)>& predicate) const
	{
		if (elemPtr and predicate(*elemPtr)) return {*elemPtr} ;
		return {nullptr};
	}

	template<
		template<typename... > class ContainerType,
		typename MappedElemType,
		typename Function = std::function<ContainerType<MappedElemType>(const ElemType&)>
		>
	Collection< ContainerType<MappedElemType> >
	flatMapWith(const Function& transform) const
	{
		if (elemPtr)
			return { transform(*elemPtr) };
		return {};
	}
};

template<
	template<typename... > class ContainerType,
	typename ElemType
	>
struct Collection< ContainerType<ElemType> >
{
	const ContainerType<ElemType> elems;

	template<
		typename MappedElemType,
		typename Function = std::function<MappedElemType(const ElemType&)>
		>
	Collection< ContainerType<MappedElemType> >
	mapWith(const Function& transform) const
	{
		ContainerType<MappedElemType> ys;

		for (const ElemType& x: elems)
			ys.push_back(transform(x));

		return {ys};
	}

	Collection< ContainerType<MappedElemType> >
	filterWith(const std::function<bool(const ElemType&)>& predicate) const
	{
		ContainerType<ElemType> ys;
		for (const ElemType& x: elems)
			if (predicate(x)) ys.push_back(x);

		return {ys};
	}

	
	template<
		typename MappedElemType,
		typename Function = std::function<ContainerType<MappedElemType>(const ElemType&)>
		>
	Collection< ContainerType<MappedElemType> >
	flatMapWith(const Function& function) const
	{
		ContainerType<MappedType> zs;
		for (const ElemType& x: elems)
			for (const MappedElemType& y: transform(x))
				zs.push_back(y);
		return {zs};
};


template<typename... Args>
struct ViewReturnType;

template<
	typename ReceivedType
	typename MappedType
	>
struct ViewReturnType< Singleton<RecievedType>, MappedType>
{
	using type = Singleton<MappedType>;
};

template<
	template<typename... > class ContainerType,
	typename ReceivedElemType,
	typename MappedElemType
	>
struct ViewReturnType< ContainerType<ReceivedElemType>, MappedElemType >
{
	using type = ContainerType<MappedType>;
};


//forward declarations
template<
	template<typename...> class ContainerType,
	typename PrevViewType,
	typename MappedType
	>
class MapHeadedView;

template<
	template<typename...> class ContainerType,
	typename PrevViewType
	>
class FilterHeadedView;

template<
	template<typename...> class ContainerType,
	typename PrevViewType,
	typename MappedType
	>
class FlatMapHeadedView;

/*
  a trait, whose code will be injected into the implementing classes
*/
template<
	template<typename...> class ContainerType,
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
		typename R  = typename std::result_of<Mapper&(MappedType)>::type,
		typename ViewType =  MapHeadedView< ContainerType, ConcreteView, R >
		>
	ViewType map(const Mapper& nextMapper) const
	{
		return ViewType(*self(), nextMapper);
	}

	template<
		typename Predicate,
		typename ViewType = FilterHeadedView< ContainerType, ConcreteView >
		>
	ViewType filter(const Predicate& pred) const
	{
		return ViewType(*self(), pred);
	}

	template<
		typename FlatMapper,
		typename CR = typename std::result_of<FlatMapper&(MappedType)>::type,
		typename R  = typename CR::value_type,
		typename ViewType = FlatMapHeadedView< ContainerType, ConcreteView, R >
		>
	ViewType flatMap(const FlatMapper& flatMapper) const
	{
		return ViewType(*self(), flatMapper);
	}



	template<typename Predicate>
	bool any(const Predicate& pred) const
	{
		for (const auto& x : self()->collect()) {
			if (pred(x)) return true;
		}
		return false;
	}
	template<typename Predicate>
	bool all(const Predicate& pred) const
	{
		for (const auto& x : self()->collect()) {
			if (not pred(x)) return false;
		}
		return true;
	}

	ContainerType<MappedType> unique() const
	{
		std::set<MappedType> xset;
		for (const auto& x : self()->collect()) xset.insert(x);
		return {xset.begin(), xset.end()};
	}
	
private :
	inline const ConcreteView* self() const
	{
		return static_cast<const ConcreteView*>(this);
	}
};


template<
	template<typename...> class ContainerType ,
	typename PrevViewType,
	typename MappedType
	//template<typename...> class ContainerType = PrevViewType::template container
	>
class MapHeadedView : public ChainableView<
	ContainerType,
	PrevViewType,
	MapHeadedView<ContainerType, PrevViewType, MappedType>,
	MappedType
	>
{
public:
	using prev_type		 = PrevViewType;
	using this_type		 = MapHeadedView<ContainerType, PrevViewType, MappedType>;
	using orig_type		 = typename prev_type::orig_type;
	using elem_type		 = typename prev_type::mapped_type;
	using mapped_type	 = MappedType;
	using recieved_type	 = typename prev_type::returned_type;
	using returned_type  = typename ViewReturnType<recieved_type, mapped_type>::type;

	using function  = std::function<MappedType(const elem_type&)>;

	template<typename... args>
	using container = typename prev_type::template container<args...>;

	MapHeadedView(const PrevViewType& prev,
				  const function& transform,
				  const container<mapped_type>& elems, 
				  const bool wasCollected) :
		_previously(prev),
		_transform(transform),
		_elements(elems),
		_hasBeenCollected(wasCollected)
	{}

	template<typename Transformer>
	MapHeadedView(const PrevViewType& prev,
				  const Transformer& transform) : 
		_previously(std::move(prev)),
		_transform(transform),
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
		_transform(std::move(that.transformer())),
		_elements(std::move(that.elements())),
		_hasBeenCollected(std::move(that.hasBeenCollected()))
	{}

    const PrevViewType& previously() const         {return _previously;      }
    const function& transformer() const            {return _transform;          }
    const container<mapped_type>& elements() const {return _elements;        }
    bool hasBeenCollected() const                  {return _hasBeenCollected;}

#if 0
	const container<mapped_type> collect() const
	{
		if (not _hasBeenCollected) {
			for (const elem_type& x : _previously.collect()) {
				_elements.push_back( _transform(x) );
			}
			_hasBeenCollected = true;
		}
		return _elements;
	}
#endif

	returned_type process(const orig_type& x) const
	{
		const recieved_type recieved = _previously.process(x);
		return recieved.mapWith(_transform);
	}
#if 0
	{
		auto yPtr = _previously.process(x);
		return yPtr
			? std::make_shared<mapped_type>(_transform(*yPtr))
			: nullptr
		;
	}
#endif
	const container<mapped_type> collect() const
	{
		container<mapped_type> zs;
		for (const orig_type& x : originalElements()) {
			const returned_type processed = process(x);
			processed.appendTo(zs);
		}
		return zs;
	}

	const container<orig_type>& originalElements()
	{
		return _previously.originalElements();
	}

private:
	const   PrevViewType           _previously;
	const   function               _transform;
	mutable container<mapped_type> _elements;
	mutable bool                   _hasBeenCollected = false;
};


template<
	template<typename...> class ContainerType,
	typename PrevViewType
	//template<typename...> class ContainerType = PrevViewType::template container
	>
class FilterHeadedView : public ChainableView<
	ContainerType,
	PrevViewType,
	FilterHeadedView<ContainerType, PrevViewType>,
	typename PrevViewType::mapped_type
	>
{
public :
	using prev_type   = PrevViewType;
	using this_type   = FilterHeadedView<ContainerType, PrevViewType>;
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

#if 0
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
#endif

	returned_type process(const orig_type& x) const
	{
		const recieved_type recieved = _previously.process(x);
		return recieved.filterWith(_predicate);
	}

	const container<elem_type> collect() const
	{
		container<elem_type> zs;
		for (const orig_type& x: originalElements()) {
			const returned_type processed = process(x);
			processed.appendTo(zs);
		}
		return zs;
	}

	const container<orig_type>& originalElements()
	{
		return _previously.originalElements();
	}

private:
	const   PrevViewType         _previously;
	const   function             _predicate;
	mutable container<elem_type> _elements;
	mutable bool                 _hasBeenCollected = true;
};

template<
	template<typename...> class ContainerType,
	typename PrevViewType,
	typename MappedElemType
	//template<typename...> class ContainerType = PrevViewType::template container
	>
class FlatMapHeadedView : public ChainableView<
	ContainerType,
	PrevViewType,
	FlatMapHeadedView<ContainerType, PrevViewType, MappedType>,
	MappedType
	>
{
public:

	using prev_type   = PrevViewType;
	using this_type   = FlatMapHeadedView<ContainerType, PrevViewType, MappedElemType>;
	using orig_type   = typename prev_type::orig_type;
	using elem_type   = typename prev_type::mapped_type;
	//using mapped_type = MappedElemType;

	template<typename... args>
	using container = typename prev_type::template container<args...>;

	using mapped_type = container<MappedElemType>;

	using function  = std::function<container<MappedElemType>(const elem_type&)>;

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

#if 0
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
#endif

	const container<mapped_type> collect() const
	{
		
	}

	const container<orig_type>& originalElements()
	{
		return _previously.originalElements();
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
	ContainerType,
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

	const container<elem_type>& elements() const { return _elements; }

	const container<elem_type>& collect() const { return _elements; }

	const container<orig_type>& originalElements() const { return _elements; }

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

}