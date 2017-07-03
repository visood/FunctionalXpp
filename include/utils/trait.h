/*
	A trait describes the desired behavior of a class,
	as well as provide the code for the desired behavior to a concrete
	implementing class.
	In C++ traits can be implemented using CRTP --- these required a static cast
	which can be provided by deriving from class trait defined below. This class
	provides two methods, "with" and "impl", that do the required cast.
*/

struct trait
{
	/*
	  Use implementation with this
	  example use-age: const Derived* implementation =  trait::with(this)
	 */
	template<
		template<typename...> class Base
		,typename Derived
		,typename... Args
		>
	static constexpr const Derived* with(const Base<Derived, Args...>* b)
	{
		return static_cast<const Derived*>(b);
	}

	/*
	  alternatively,  we can use the implementation's code at the point of use in
	  the definition of the trait
	  example use-age : impl(this)->doSomething
	*/

	template<
		template<typename...> class Base
		,typename Derived
		,typename... Args
		>
	static constexpr const Derived* impl(const Base<Derived, Args...>* b)
	{
		return static_cast<const Derived*>(b);
	}
};
