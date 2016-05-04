#include <array>
#include <iterator>

template <typename T >
class Tree {
  using ptr_t = std::shared_ptr<Tree>;
public:
Tree(const T v) : _left(), _parent(), _right(), _value(v) {}
Tree(ptr_t l, ptr_t r, const T v): _left(l), _parent(), _right(r), _value(v) {}

  const ptr_t left {return _left;}
  const ptr_t right {return _right;}
  const ptr_t parent{return _parent;}
  const bool visited{return _visited;}
  class NodeIterator : public std::iterator<std::input_iterator_tag, T> {
  public:
  NodeIterator(const ptr_t root ) : _root(root);

    ptr_t root() { return _root;}
    ptr_t crtpos() { return _crtpos;}

    NodeIterator& next(const ptr_t pos) {
      if (pos) {
        auto vt = traverse(pos);
        _crtval = vt.first;
        _crtpos = vt.second;
      }
      return *this;
    }
    NodeIterator& start() {
      return next(_root);
    }
    NodeIterator& next() {
      if (_crtpos) {
        auto vt = traverse(_current);
        _crtval = vt.first;
        _crtpos = vt.second;
      }
      return *this;
    }

    bool done() {
      _crtpos ? false : true;
    }

    bool operator ==(const NodeIterator& that) {
      return _root == that.root() and _ctrpos == that.crtpos();
    }

    bool operator !=(const NodeIterator& that) {
      return not (*this) == that;
    }

    private:
    const Tree::ptr_t _root = nullptr;
    Tree::ptr_t _crtpos = nullptr;
    T_ _crtval;
  };
private:
  ptr_t _left = nullptr;
  ptr_t _parent = nullptr;
  ptr_t _right = nullptr;
  const T _value;
  bool _visited = false;
};


template <typename T>
std::pair<T, const Tree<T>::ptr_t > traverse(Tree<T>::ptr_t from) {
  if (from->left and not from->left->visited)
    return traverse(from->left);
  if (from->right and not from->right->visited) {
    from->visited = true;
    return std::make_pair(from->value, from->right);
  }
  if (from->visited)
    return traverse(from->parent);
  from->visited = true;
  return std::make_pair(from->value, from->parent);
}

template <typename T>
std::vector<T> fringe_values(const Tree<T>::ptr_t tree) {
  std::vector<T> fringe;
  std::pair<T, Tree<T>::ptr_t> vt = traverse(tree);
  do {
    fringe.push_back(vt.first);
    auto vt = traverse(vt.second);
  } while (vt.second);
}

