#include <limits>
#include <dirent.h>
#include <stdexcept>

std::vector<std::string> getdir(std::string dir) {
    DIR *dp;
    struct dirent *dirp;
    std::vector<std::string> files;
    if ((dp = opendir(dir.c_str())) == NULL) {
        std::cout << "Error: directory " << dir << " could not be opened." << std::endl;
    } else {
        while ((dirp = readdir(dp)) != NULL) {
            files.push_back(std::string(dirp->d_name));
        }
        closedir(dp);
    }
    return files;
}

template<typename ContainerOut, typename ContainerIn >
ContainerOut collect(ContainerIn xs, ContainerOut ys) {
    ys.resize(xs.size());
    std::copy(std::begin(xs), std::end(xs), std::begin(ys));
    return ys;
}

template< typename T >
const T id(const T& t) { return t;}

template< typename T >
T id(T& t) { return t;}

template<typename UnaryOp, typename T>
class Xrange {
public:
Xrange(int32_t low, int32_t high):
  _low((high - low) >= 0 ? low : high),
    _high((high - low) >= 0 ? high : low),
    _delta((high - low) >= 0 ? 1 : -1),
    _apply(id<int32_t>) {}

Xrange(int32_t low, int32_t high, int32_t delta):
  _low(delta*(high - low) >= 0 ? low : high),
    _high(delta*(high - low) >= 0 ? high : low ),
    _delta(delta),
    _apply(id<int32_t>) {}

Xrange(F f, int32_t low, int32_t high):
  _low((high - low) >= 0 ? low : high),
    _high((high - low) >= 0 ? high : low),
    _delta((high - low) >= 0 ? 1 : -1),
    _apply(f) {}

Xrange(F f, int32_t low, int32_t high, int32_t delta):
  _low(delta*(high - low) >= 0 ? low : high),
    _high(delta*(high - low) >= 0 ? high : low ),
    _delta(delta),
    _apply(f) {}

    class Iterator {
    public:
    Iterator(uint32_t pos, int32_t delta) : _pos(pos), _delta(delta) {}
      using iterator_category = std::forward_iterator_tag;
      using value_type = T;
      using difference_type = int32_t;
      using pointer = T*;
      using reference = T&;
      uint32_t pos() const { return _pos; }
      int32_t delta() const {return _delta; }
      const T& operator*() const { return _current; }
      Iterator& operator++() {
        _pos += _delta;
        _current = _apply(_pos);
        return *this;
      }

      bool operator!=(const Iterator& that) {
        return _pos != that.pos() or _delta != that.delta();
      }

      bool operator==(const Iterator& that) {
        return _pos == that.pos() and _delta == that.delta();
      }

    private:
        uint32_t _pos = 0;
        int32_t _delta = 1;
        const static UnaryOp _apply;
        T current = _apply(_pos);
    };


    int32_t low() const { return _low; }
    int32_t high() const { return _high; }
    int32_t delta() const {return _delta;}

    int32_t size() { return _delta > 0 ? (_high - _low)/_delta : (_low - _high)/(-_delta); }

    int32_t length() { return size(); }

    Iterator begin() { return Iterator( _low, _delta); }
    Iterator end() { return Iterator(_high, _delta); }


private:
    int32_t _low = 0;
    int32_t _high = 0;
    int32_t _delta = 1;
};

template< class Container >
Container Range(uint32_t low, uint32_t high, int32_t delta) {
    Xrange xr(low, high, delta);
    Container out(xr.size());
    return collect(Xrange(low, high, delta), out);
}
