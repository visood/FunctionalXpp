
/*
  a trait that specifies a chainable view
  no constructors here
*/
#include <set>

namespace view {

template<
	template<typename...> class C,
	typename P,
	typename M
	> class MapHeadedView;
template<
	template<typename...> class C,
	typename P
	> class FilterHeadedView;
template<
	template<typename...> class C,
	typename P,
	typename M
	>  class FlatMapHeadedView;

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
	using prev_type   = PrevViewType;
	using this_type   = MapHeadedView<ContainerType, PrevViewType, MappedType>;
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
	template<typename...> class ContainerType,
	typename PrevViewType,
	typename MappedType
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
	using this_type   = FlatMapHeadedView<ContainerType, PrevViewType, MappedType>;
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

} //namespace view
