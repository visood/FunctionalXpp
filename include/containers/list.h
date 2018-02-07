#include<iostream>
#include<memory>
#include<stdexcept>

namespace Persistent{ namespace List{

template<typename ElemType>
class ConsCell
{
public:
	using this_type = ConsCell<ElemType>;

	ConsCell() = default;
	ConsCell(const ElemType& x, //cannot have a reference here -- unique pointer will own
					 const std::shared_ptr<this_type>& next) :
		_elem(std::make_unique<ElemType>()),
		_next(next)
	{
		*_elem = x;
	}
	const ElemType& elem() const {return *_elem;}
	const this_type& next() const {return *_next;}
	const std::shared_ptr<this_type>& nextPtr() const { return _next;}
	//const std::shared_ptr<const this_type>& next() const {return _next;}

	bool empty() const {return _elem ? false : true;}

private:
	std::unique_ptr<ElemType>  _elem; //this should be nullptr by default
	std::shared_ptr<this_type> _next; //this should be nullptr by default
};

template<typename ElemType>
const ConsCell<ElemType> Nil; /*can this be constexpr?*/

template<typename ElemType>
class List
{
public:
	using ConsCellType = ConsCell<ElemType>;

	List()
	{
		const auto hc = std::make_shared<ConsCellType>();
		*hc = ConsCellType();
		_headCell = hc;
	}

	List(const std::shared_ptr<ConsCellType>& headCell):
		_headCell(headCell)
	{}

	bool empty() const {return _headCell->empty();}

	int size() const {return empty() ? 0 : 1 + tail().size();}

	const ConsCellType& headCell() const {return *_headCell; }

	const ElemType& head() const { return _headCell->elem();}

	List<ElemType> tail() const
	{
		if (empty()) 
			throw std::out_of_range("tail called on an empty list !!!");

		return List<ElemType>(_headCell->nextPtr());
	}

	List<ElemType> cons(const ElemType& x) const
	{
		std::cout << "cons " << x << " onto a list" << std::endl
							<< "with head element ";
		if (_headCell and not _headCell->empty())
			std::cout << _headCell->elem();
		else
			std::cout << "null";

		std::cout << std::endl
							<< " and head cell used in "
							<< _headCell.use_count() << " instances."
							<< std::endl;
		const auto xPtr = std::make_shared<ConsCellType>();
		*xPtr = ConsCellType(x, _headCell);
		std::cout << "consed, head used in "
							<< _headCell.use_count() << " instances."
							<< std::endl;
		return List<ElemType>(xPtr);
	}

	void printCellUsage() const
	{
		if (empty())
			std::cout << "done" << std::endl;
		else {
			std::cout << "head with elem " << _headCell->elem() << ", "
								<< "cell usage: " << _headCell.use_count() << ", ";
			std::cout << "( head-cell's next has usage "
								<< _headCell->nextPtr().use_count()
								<< "); ";
			tail().printCellUsage();
		}
	}
	
private:
	std::shared_ptr<ConsCellType> _headCell;
	//static const ConsCellType _nil;
};

//template<typename ElemType>
//const ConsCell<ElemType> List<ElemType>::_nil = ConsCell<ElemType>();

template<typename ElemType>
List<ElemType> cons(const ElemType& x, const List<ElemType>& ys)
{
	return ys.cons(x);
}



} /*namespace List*/ } /*namespace Persistent*/

