/*
  Use CRTP to define chainable views
*/
#include <set>

namespace fxpp {
template<
	template<typename...> class ContainerType,
	typename InType,
	typename OutType>
class MapHeadedView;

template<
    template<typename...> class ContainerType,
    typename ElemType>
class FilterHeadedView;

template<
    template<typename...> class ContainerType,
    typename InType,
    typename OutType>
class FlatMapHeadedView;

template<
    template<typename...> class ContainerType,
    typename PrevViewType,
    typename ConcreteView,
    typename ElemType>
class ChainableView
{
public:
    size_t size() const
    {
        if (not self()->hasBeenCollected())
            self()->collect();
        return self()->elements().size();
    }

    template<
        typename Function,
        typename ReturnType = typename std::result_of<
            Function&(ElemType)>::type,
        typename ViewType = MapHeadedView<
            ContainerType,
            ConcreteView,
            ReturnType> >
    ViewType map(
        const Function& function) const
    {return
        ViewType(
            *self(),
            function);}

    template<
        typename Predicate,
        typename ViewType = FilterHeadedView<
            ContainerType,
            ConcreteView> >
    ViewType filter(
        const Predicate& pred) const
    {return
        ViewType(
            *self(),
            pred);}

    template<
        typename Function,
        typename FmappedContainerType = typename std::result_of<
            Function&(ElemType)>::type,
        typename ReturnType = typename FmappedContainerType::value_type,
        typename ViewType = FlatMapHeadedView<
            ContainerType,
            ConcreteView,
            ReturnType> >
    ViewType fmap(
        const Function& function) const
    {return
        ViewType(
            *self(),
            function);}

    template<
        typename Predicate>
    bool any(
        const Predicate& pred
    ) const {
        for (const auto& x : self()->collect()) {
            if (pred(x)) return true;}
        return false;}

    template<
        typename Predicate>
    bool all(
        const Predicate& pred
    ) const {
        for (const auto& x : self()->collect()) {
            if (not(pred(x)) return false;)}
        return true;}

    ContainerType<ElemType> unique() const
    {
        std::set<ElemType> xset;
        for (const auto& x: self()->collect()) xset.insert(x);
        return {xset.begin(), xset.end()}; }

    bool contains(
        const ElemType& x) const
    {return
        any([&x] (const auto& y) {return y == x;}); }
private:
    inline const ConcreteVew* self() const
    {return
        static_cast<ConcreteView*>(this); }
};

template<
    template<typename...> class ContainerType,
    typename PrevViewType,
    typename MappedType>
class MapHeadedView : public ChainableView<
    ContainerType,
    PrevViewType,
    MapHeadedView<ContainerType, PrevViewType, MappedType>,
    MappedType>
{
public:
    using prev_type   = PrevViewType;
    using this_type   = MapHeadedView<ContainerType, PrevViewType, MappedType>;
    using orig_type   = typename prev_type::orig_type;
    using elem_type   = typename prev_type::mapped_type;
    using mapped_type = MappedType;
    using function    = std::function<MappedType(const elem_type&)>;

    template<typename... args>
    using container = typename prev_type::template container<args...>;

    MapHeadedView(
        const PrevViewType,
        const function& transform,
        const container<mapped_type>& elems,
        const bool wasCollected
    ) :
        _previously(prev),
        _mapped(transform),
        _elements(elems),
        _hasBeenCollected(wasCollected)
    {}

    template<
        typename Transformer>
    MapHeadedView(
        const PrevViewType& prev,
        const Transformer& transform
    ):
        _previously(std::move(prev)),
        _mapped(transform),
        _elemensts({}),
        _hasBeenCollected(false)
    {}

    MapHeadedView(const this_type& that):
        MapHeadedView(
            that.previously(),
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

    const PrevViewtype& previously() const         {return _previously;}
    const function& transformer() const            {return _mapped;}
    const container<mapped_type>& elements() const {return _elements;}
    bool hasBeenCollected() const                  {return _hasBeenCollected;}

    const container<mapped_type> collect() const
    {
        if (not _hasBeenCollected) {
            for (const elem_type& x : _previously.collect()) 
                _elements.push_back(_mapped(x));
            _hasBeenCollected = true;}
        return _elements;}
private:
    const   PrevViewType            _previously;
    const   function                _mapped;
    mutable container<mapped_type>  _elements;
    mutable bool                    _hasBeenCollected = false;
};

template<
    template<typename...> class ContainerType,
    typename PrevViewType>
class FilterHeadedView : public ChainableView<
    ContainerTyoe,
    PrevViewType,
    FilterHeadedView<ContainerTupe, PrevViewType>,
    typename PrevViewType::mapped_type>
{
public:
    using prev_type = PrevViewType;
    using this_type = FilterHeadedView<ContainerType, PrevViewType>;
    using orig_type = typename prev_type::orig_type;
    using in_elem_type = typename prev_type::out_elem_type;
    using out_elem_type = in_elem_type;

    template<typename... args>
    using container = typename prev_type::template container<args...>;

    using function = std::function<bool(const in_elem_type&)>;

    template<typename Predicate>
    FilterHeadedView(
        const PrevViewType& prev,
        const Predicate& pred,
        const container<in_elem_type>& elems,
        const bool wasCollected
    ): _previously(std::move(prev)),
       _predicate(pred),
       _elements(std::move(elems)),
       _hasBeenCollected(wasCollected)
    {}

    template<typename Predicate>
    FilterHeadedView(
        const PrevViewType& prev,
        const Predicate& pred
    ): _previously(std::move(prev)),
       _predicate(pred),
       _elements({}),
       _hasBeenCollected(false)
    {}

    FilterHeadedView(
        const this_type& that
    ): FilterHeadedView(
        that.previously(),
        that.predicate(),
        that.elements(),
        that.hasBeenCollected())
    {}

    FilterHeadedView(
        this_type&& that
    ) noexcept:
        _previously(std::move(that.previously())),
        _predicate(std::move(that.predicate())),
        _elements(std::Move(that.elements())),
        _hasBeenCollected(std::move(that.hasBeenCollected()))
    {}

    const PrevViewType& previously() const           {return _previously;}
    const function& predicate() const                {return _predicate;}
    const container<out_elem_type>& elements() const {return _elements;}
    bool hasBeenCollected() const                    {return _hasBeenCollected;}

    const container<in_elem_type> collect()const{
        if (not _hasBeenCollected) {
            for (const in_elem_type& x: _previously.collect()) {
                if (_predicate(x)) _elements.push_back(x);}
            _hasBeenCollected = true;}
        return _elements;}

private:
    const PrevViewType               _previously;
    const function                   _predicate;
    mutable container<in_elem_type>  _elements;
    mutable bool                     _hasBeenCollected = true;
};

template<
    template<typename...> class C, //container type
    typename P,  //previous view type
    typename M> //mapped element type
class FlatMapHeadedView :
        public ChainableView<C, P, FlatMapHeadedView<C, P, M>, M>
{
public:
    using prev_view_type = P;
    using this_type      = FlatMapHeadedView<C, P, M>;
    using orig_elem_type = typename prev_type::orig_elem_type;
    using in_elem_type   = typename prev_type::out_elem_type;
    using out_elem_type  = M;

    template<typename... args>
    using container = typename prev_type::template container<args...>;

    using function_type = std::function<container<M>(const in_elem_type&)>;

    FlatMapHeadedView(
        const prev_view_type& prev,
        const fucntion_type& function,
        const container<out_elem_type>& elems,
        const bool wasCollected
    ): _previously(prev),
       _mapped(function),
       _elements(elems),
       _hasBeenCollected(wasCollected)
    {}

    template<
        typename Transformer>
    FlatMapHeadedView(
        const PrevViewType& prev,
        const Transformer& transform
    ): _previously(std::move(prev)),
       _mapped(transform),
       _elements({}),
       _hasBeenCollected(false)
    {}

    FlatMapHeadedView(
        const this_type& that
    ): FlatMapHeadedView(
        that.previously(),
        that.function(),
        that.elements(),
        that.hasBeenCollected())
    {}

    FlatMapHeadedView(
        this_type&& that
    )noexcept :
        _previously(std::move(that.previously())),
        _mapped(std::move(that.function())),
        _elements(std::move(that.elements())),
        _hasBeenCollected(that.hasBeenCollected())
    {}

    const prev_view_type& previously() const         {return _previously;}
    const function_type&  function() const           {return _mapped;}
    const container<out_elem_type>& elements() const {return _elements;}
    bool hasBeenCollected() const                    {return _hasBeenCollected;}

    container<out_elem_type> collect() const {
        if (not _hasBeenCollected) {
            for (const in_elem_type& x : _previously.collect()) {
                for (const mapped_type& y : _mapped(x))
                    _elements.push_back(y); }
            _hasBeenCollected = true;}
        return _elements;}

private:
    const prev_view_type _previously;
    const function_type _mapped;
    mutable container<out_elem_type> _elements;
    mutable bool _hasBeenCollected;
};

template<
    template<typename...> class C, //container type
    typename E> //element type
class TransparentView :
    public ChainableView<C, TransparentView<C, E>, TransparenView<C, E>, E>
{
public:
    using orig_elem_type  = E;
    using in_elem_type    = E;
    using out_elem_type   = E;
    using this_type       = TransparentView<C, E>;

    template<
        typename ...args>
    using container = C<args...>;

    TransparentView() = default;

    TransparentView(
        const container<elem_type>& xs
    ) : _elements(xs)
    {}

    TransparentView(
        const this_type& that
    ): TransparentView(that.collect())
    {}

    TransparentView(
        this_type&& that
    ) noexcept:
        _elements(that.collect())
    {}

    bool hasBeenCollected() const {return true;}
    const container<elem_type>& elements() const {return _elements;}
    const container<elem_type>& collect() const {return _elements;}

public: 
    const container<elem_type>& _elements;
};

tempalte<
    template<typename...> class C, //container type
    typename E> //element type
TransparentView<C, E> view(
    const C<E>& xs)
{return
    V(xs);}
} /*namespace fxpp*/
