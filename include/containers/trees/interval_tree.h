#include <array>
#include <iterator>


class Direction;

class Left : public Direction;

class Center : public Direction;

class Right : public Direction;

template <typename Position>
class Interval {
public:
  static Position leftEnd(std::tuple<Position, 2> xy) {
    return std::min(std::get<0>(xy), std::get<1>(xy));
  }
  static Position rightEnd(std::tuple<Position, 2> xy) {
    return std::max(std::get<0>(xy), std::get<1>(xy));
  }

  static Direction direction(const Interval xy, const Position c) {
    if (rightEnd(xy) < c) return Left;
    if (leftEnd(xy) >= c) return Right;
    return Center;
  }
};

class IntervalSet {}

template < typename Position>
class IntervalTree {
  using IntervalType = std::tuple<Position, 2>;
  using ptr_t = std::shared_ptr<IntervalTree>;
public:
IntervalTree(Position x) :
  _left(), _parent(), _right(), _X(x) {}
IntervalTree(ptr_t l, ptr_t r, const Position x) :
  _left(l), _parent(), _right(r), _X(v) {}

  //getters
  const ptr_t const left() {return _left; }
  const ptr_t const parent() {return _parent; }
  const ptr_t const right() {return _right; }
  const std::set<IntervalType>& intervalSet() { return _intervals;}

  ptr_t insert(const Interval xy) const {
    //return insert_at[Interval::direction(xy, c)];
    if (Interval::rightEnd(xy) < _X) return _left->insert(xy)
    if (Interval::leftEnd(xy) >= _X) return _right->insert(xy);
    _intervalBegins.insert(xy);
    _intervalEnds.insert(xy);
  }

  ptr_t insert_center
private:
  ptr_t _left = nullptr;
  ptr_t _parent = nullptr;
  ptr_t _right = nullptr;
  const Position _X;
  std::set<IntervalType> _intervalBegins;
  std::set<IntervalType> _intervalEnds;
};
