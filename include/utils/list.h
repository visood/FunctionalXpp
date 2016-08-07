//a monadic list
//an example to design an immutable, persistent data structure

#pragma once
#include "util.h"

#if 0
//a bit daunting because i do not understand the behavior of shared ptr
//but a promising way to learn
template<typename T>
class Cell
{
public:
	explicit Cell(const T& t) :
		_datum(t),
		_next(nullptr)
	{}
	explicit Cell(const T& t, const auto nptr) :
		_datum(t),
		_next(nptr)
	{}
	
	const std::shared_ptr< Cell<T> >  next() const { return _next;}
	Cell<T> link(const Cell<T>& c) { _next = &c;}

private:
	T _datum;
	std::shared_ptr< Cell<T> > _next;
}

template<typename T>
class List
{
	using type = T;
	std::shared_ptr< Cell<T> > _root;
	std::shared_ptr< Cell<T> > _tip;

public:
	explicit List() :
		_root { nullptr },
		_tip { nullptr }
	{}
	explicit List(const Cell<T>& n, const Cell<T>& t) :
		_root { &n },
		_tip { &t }
	{}
	explicit List(const T& t) :
		_root {std::make_shared< Cell<T> >(Cell<T>(t));}
		_tip { _root}
	{}
	explicit List(const T& t, const Cell<T>& c) :
		List( std::make_shared< Cell<T> > (Cell<T>(t, _root)), _tip)
	{}

	
	~List()
	{}

	bool nil() {return (_root == nullptr);}

	const Cell<T>& root() { return _root;}
	const Cell<T>& tip() { return _tip;}

	void push_front(const T& t)
	{
		_root = std::make_shared<Cell>(Cell(t, _root));
		//std::unique_ptr<Cell> new_node(new Cell(t, _root));
		//new_node->next = _root;
		//_root = std::move(new_node);
	}
	void push_back(const T& t)
	{
		auto n = std::make_shared< Cell<T> >(Cell(t));
		_tip->next = n;
		_tip = n;
	}

	void append(const List<T>& that)
	{
		if (that.nil()) return;
		push_back(that.head());
		return append(that.tail());
	}

	T head() const { return _root->datum;}
	const T& head() const {return _root->datum;}
	List<T> tail() const
	{
		return List<T>(_root->next;)
	}

	//if we do not want a push_back function,
	//we will need a method to reverse the List
	List<T> reverse() const
	{
		List<T> rts();
		auto n = _root;
		while(n != nullptr) {
			rts.push_front(n->datum);
			n = n->next;
		}
		return rts;
	}
	//monadic return
	static unit(const T& t)
	{
		List<T> l();
		l.push_front(t);
		return l;
	}
	//map the list
	template<
		typename F,
		typename S = typename std::result_of<F&(T)>::type
	>
	List<S> map(const F& f)
	{
		List<S> ss;
		auto n = _root;
		while (n != nullptr) {
			ss.push_back(f(n->datum));
		}
		return ss;
	}
	//monadic bind
	template<
		typename F,
		typename LS = typename std::result_of<F&(T)>::type,
		typename S = typename LS::type
	>
	List<S> operator >>= (const F& f)
	{
		List<S> lss;
		auto n = _root;
		while (n != nullptr) {
			lss.append(f(n->datum));
		}
		return lss;
	}

}

template<typename T>
const List<T> nil = List<T>();

template<typename T>
List<T> operator >> (const T& t, const List<T>& ts)
{
	return List<T>(t, ts.root());
}

#endif
