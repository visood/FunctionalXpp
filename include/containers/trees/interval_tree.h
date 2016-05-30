#include <array>
#include <iterator>



namespace Interval {

  template <typename Position>
  Position beginning(const Position x, const Position y)
  {
    return std::min(x, y);
  }
  template <typename Position>
  Position ending(const Position x, const Position y)
  {
    return std::max(x, y);
  }
  template< typename Position >
  Position beginning(const std::tuple<Position, 2> xy)
  {
    return std::min(std::get<0>(xy), std::get<1>(xy));
  }
  template< typename Position >
  Position ending(const std::tuple<Position, 2> xy)
  {
    return std::max(std::get<0>(xy), std::get<1>(xy));
  }

  template< template<typename...> typename Interval, typename Position >
  Position center(Interval<Position> xy)
  {
    return (ending(xy) + beginning(xy))/2;
  }


  template <typename Position>
  Position beginning (const Position x) const { return x;}

  template <typename Position >
  Position ending (const Position x) const { return x + 1;}

  template <typename QueryType>
  Position length(const QueryType xy) const
  {
    return ending(xy) - beginning(xy);
  }

  enum class Direction {Left = -1, Center = 0, Right = 1};

  //template < template <typename...> typename Interval,  typename Position >
  template <typename QueryType>
  Direction direction(const Querytype x, const Position c)
  {
    if (ending(x) <= c) return Direction::Left;
    if (beginning(x) > c) return Direction::Right;
    return Direction::Center;
  }

  template <template<typename...> typename Interval, typename Position>
  class  Ordering
  {
  public:
    enum class Sense {Increasing, Decreasing};
    using Itype = Interval<Position>;
    static bool operator () (const Position x, const Position y,
                             const Sense s = Sense::Increasing)
    {
      switch (s) {
      case Sense::Increasing :
        return x < y;
      case Sense::Decreasing :
        return x >= y;
      }
    }

    static bool Begin(const Itype uv, const Itype xy,
                      const Sense s = Sense::Increasing)
    {
      if (beginning(uv) == beginning(xy))
        return Ordering(ending(uv), ending(xy), sense);
      return Ordering(beginning(uv), beginning(xy), sense);
    };

    static bool End(const Itype uv, const Itype xy,
                    const Sense s = Sense::Increasing)
    {
      if (ending(uv) == ending(xy))
        return Ordering(beginning(uv), beginning(xy), sense);
      return Ordering(ending(uv), ending(xy), sense);
    };

    static bool Mid(const Itype uv, const Itype xy,
                    const Sense s = Sense::Increasing)
    {
      if (center(uv) == center(xy))
        return Ordering(length(uv), length(uv), sense);
      return Ordering(center(uv), center(xy), sense);
    };

    static bool Length(const Itype uv, const Itype xy,
                       const Sense s = Sense::Increasing)
    {
      if (length(uv) == length(xy))
        return Ordering(center(xy), center(uv), sense);
      return Ordering(length(uv), length(xy), sense);
    }

  };

  template <template<typename...> typename Interval, typename Position>
  class set
  {
  public:
    using Itype = Interval<Position>;

    uint size() const
    {
      if (_begins.size() != _ends.size()) {
        throw std::logic_error("bset size" + _begins.size() +
                               "not equal to end set size" + _ends.size());
      }
      return _bset.size();
    }

    void insert (const Itype uv)
    {
      _begins.insert(uv);
      _ends.insert(uv);
    }

    std::vector<const Itype> intersecting(const Position x) const
    {
      std::vector<const Itype> xys;
      switch (direction(x, _X) ) {
      case Direction::Left :
        std::copy(begin(_begins), _begins.upper_bound(ending(x)), std::back_inserter(xys));
        break;
      case Direction::Center :
        break;
      case Direction::Right :
        std::copy(_ends.lower_bound(beginning(x)), end(_ends), std::back_inserter(xys));
        break;
      }
      return xys;
    }

    std::vector<const Itype> intersecting(const Itype xy) const
    {
      std::vector<const Itype> xys;
      switch (direction(xy, _X)) {
      case Direction::Left :
        std::copy(begin(_begins), _begins.upper_bound(ending(xy)), std::back_inserter(xys));
        break;
      case Direction::Center :
        std::copy(begin(_begins), end(_begins), std::back_inserter(xys));
        break;
      case Direction::Right :
        std::copy(_ends.lower_bound(beginning(xy)), end(_ends), std::back_inserter(xys));
        break;
      }
      return xys;
    }

  private:
    const Position _X;
    std::set<const Itype, Ordering::Begin> _begins;
    std::set<const Itype, Ordering::End> _ends;
  };

  template <template<typename...> typename Interval, typename Position>
  class Tree {
  public:
    using Itype = Interval<Position>;
    using ptr_type = std::shared_ptr<Tree>;
  Tree(const Position x) :
    _left(), _parent(), _right(), _X(x) {}
  Tree(ptr_type l, ptr_type r, const Position x) :
    _left(l), _parent(), _right(r), _X(x);

    //getters
    const ptr_type left() const {return _left;}
    const ptr_type parent() const {return _parent;}
    const ptr_type right() const {return _right; }


    const ptr_type insert(const Itype xy)
    {
      switch (Interval::direction(xy, _X))
      {
      case Interval::Left :
        return _left->insert(xy);
      case Interval::Center :
        _intervals.insert(xy);
        return this;
      case Interval::Right :
        return _right->insert(xy);
      default : throw(
        std::invalid_argument("Unknown Direction for interval to insert")
        );
      }
    }


    template <QueryType x>
    std::vector<const Itype> intersecting(const QueryType x)
    {
      std::vector<const Itype> segments = _intervals.intersecting(x, );
      std::vector<const Itype> fromDescendants;
      switch (direction(x, _X)) {
      case Direction::Left :
        if (_left) fromDescendants = _left->intersecting(xy);
        break;
      case Direction::Right :
        if (_right) fromDescendants = _right->intersecting(xy);
        break;
      case Direction::Center:
        break;
      }
      std::copy(begin(fromDescendants), end(fromDescendants), end(segments));
      return segments;
    }


  private:
    ptr_type _left = nullptr;
    ptr_type _parent = nullptr;
    ptr_type _right = nullptr;
    const Position _X;
    Interval::set<Interval, Position> _intervals;
  };

}
