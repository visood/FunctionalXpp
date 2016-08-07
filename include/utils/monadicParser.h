
//for the parser to be a monad we follow haskell

//a monad of type T requires a 'return', which we implement
//as a function yield, outside any class

namespace Monadic {
using String = std::string;
// a wrapper around std::list that employs monadic relationships
template <typename T>
using List = std::list<T>;



//from a parser, we need to return the result of its action on string

template< typename T >
class Parser
{
public:

	template <typename S>
	Parser<S> flatMap( std::function<Parser<S>(T)> fpst) const
	{
		return Parser<S> (
			[=] (const String& in) -> ParsedResult<S> {
				const auto rt = parse(_apply, in);
				if (rt.empty) return nothing<T>;
				return parse(fpst(rt.value), rt.out);
			}
		);
	}

	template <typename S>
	Parser<S> map( std::function<S(T)> fst) const
	{
		return Parser<S> (
			[=] (const String& in) -> ParsedResult<S> {
				const auto rt = parse(_apply, in);
				if (rt.empty) return nothing<T>;
				return result(fst(rt.value), rt.out);
			}
		);
	}

	//seemingly map can be written using a flatMap!
	template <typename S>
	Parser<S> mapy( std::function<S(T)> fst) const
	{
		return flatMap( [=] (const T t) {
				yield (t);
			}
		);
	}


private:
	std::function<ParsedResult<T>(String)> _apply;
};

//monadic return must sit outside the class as a function
template<typename T>
Parser<T> unit(const T& t)
{
  return Parser<T>(
    [=] (const String& in) {return some(t, in);}
  );
}


//a helper function to construct Parser
template <
	typename Function,
	typename T = typename std::result_of<Function&(String)>::type::data_type
>
Parser<T> parser(const Function f)
{
	return Parser<T> (f);
}
//an example, assuming word has been defined
using TwoWordTup = std::tuple<Word, Word>;
const auto twoWords = word.flatMap([=] (const Word& w1) {
		return word.map([=] (const Word& w2) {
				return std::make_tuple(w1, w2);
			}
		);
	}
);
//replacing map with flatMap and yield
const auto twoWords = word.flatMap( [=] (const Word& w1) {
		return word.flatMap( [=] (const Word& w2) {
				return yield(std::make_tuple(w1, w2));
			}
		);
	}
);

//using operators, haskell style
const auto twoWords = word >>= [=] (const Word& w1) {
	return word >>= [=] (const Word& w2) {
		return yield(std::make_tuple(w1, w2));
	};
};

const auto threeWords = word.flatMap ( [=] (const Word& w1) {
		return word.flatMap ( [=] (const Word& w2) {
				return word.map ([=] (const Word& w3) {
						return std::make_tuple(w1, w2, w3);
					}
				);
			}
		);
	}
);

//a generic repeat
template<typename T>
auto three(const Parser<T>& pt)
{
	return pt >>= [=] (const T& t1) {
		return pt >>= [=] (const T& t2) {
			return pt >>= [=] (const T& t3) {
				return yield(std::make_tuple(t1, t2, t3));
			}
		};
	};
}

//to understand monad further, lets implement a List Monad
//we should implement the List Monad to take any container

template< typename T, template <typename... > C>
class ListMonad
{

}

} /* namespace Monadic */
