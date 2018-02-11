#include<iostream>
#include<memory>
#include<stdexcept>

namespace Persistent{ namespace List{

template<typename ElemType>
class List;

template<typename ElemType>
class ConsCell
{
public:
	using this_type = ConsCell<ElemType>;

	ConsCell() = default;
	ConsCell(const ElemType& x,
					 const std::shared_ptr<this_type>& nextPtr) :
		_elemPtr(std::make_unique<ElemType>()),
		_nextPtr(nextPtr)
	{
		*_elemPtr = x;
	}
	const ElemType&  elem() const {return *_elemPtr;}
	const this_type& next() const {return *_nextPtr;}

	bool empty() const {return _elemPtr ? false : true;}

	friend class List<ElemType>;
private:
	std::unique_ptr<ElemType>  _elemPtr; //this should be nullptr by default
	std::shared_ptr<this_type> _nextPtr; //this should be nullptr by default
};

template<typename ElemType>
const ConsCell<ElemType> Nil; /*can this be constexpr?*/

template<typename ElemType>
class List 
{
public:
	using ConsCellType = ConsCell<ElemType>;

	List() :
		_headCellPtr(std::make_shared<ConsCellType>())
	{
		//const auto hc = std::make_shared<ConsCellType>();
		//*hc = ConsCellType();
		*_headCellPtr = ConsCellType();
	}

	List(const std::shared_ptr<ConsCellType>& headCellPtr):
		_headCellPtr(headCellPtr)
	{}

	bool empty() const {return _headCellPtr->empty();}

	bool nil() const {return _headCellPtr->empty();}

	int size() const {return empty() ? 0 : 1 + tail().size();}

	const ElemType& head() const { return *(_headCellPtr->_elemPtr); }

	List<ElemType> tail() const
	{
		if (empty()) 
			throw std::out_of_range("tail called on an empty list !!!");

		return List<ElemType>(_headCellPtr->_nextPtr);
	}

	List<ElemType> cons(const ElemType& x) const
	{
		const auto xPtr = std::make_shared<ConsCellType>();
		*xPtr = ConsCellType(x, _headCellPtr);
		return List<ElemType>(xPtr);
	}

	void printCellUsage() const
	{
		if (empty())
			std::cout << "done" << std::endl;
		else {
			std::cout << "head with elem " << *(_headCellPtr->_elemPtr) << ", "
								<< "cell usage: " << _headCellPtr.use_count() << ", ";
			std::cout << "( head-cell's next has usage "
								<< _headCellPtr->_nextPtr.use_count()
								<< "); ";
			tail().printCellUsage();
		}
	}
	
private:
	std::shared_ptr<ConsCellType> _headCellPtr;
};

template<typename ElemType>
const ElemType& head(const List<ElemType>& l) {return l.head();}

template<typename ElemType>
const List<ElemType> tail(const List<ElemType>& l) { return l.tail(); }

template<typename ElemType>
List<ElemType> cons(const ElemType& x, const List<ElemType>& ys)
{
	return ys.cons(x);
}

template<typename ElemType>
List<ElemType> make_list(const ElemType& x)
{
	return cons(x, List<ElemType>());
}
template<typename HeadType, typename... TailTypes>
List<HeadType> make_list(const HeadType& h, TailTypes... tailArgs)
{
	return cons(h, make_list(tailArgs...));
}
} /*namespace List*/ } /*namespace Persistent*/

template<
	typename ElemType,
	typename ListType = Persistent::List::List<ElemType>
	>
inline const ListType operator >> (const ElemType& head, const ListType& ys)
{
	return ys.cons(head);
}

