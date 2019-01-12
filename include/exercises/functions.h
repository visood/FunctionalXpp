//some code we wrote a while ago, to understand functional implementations
//in C++.
inline std::vector<std::string> getdir (std::string dir )
{
  DIR *dp;
  struct dirent *dirp;
  std::vector<std::string> files;
  if ((dp = opendir(dir.c_str())) == NULL) {
    std::cout << "Error: directory "
							<< dir << " could not be opened."
							<< std::endl;
  } else {
    while ((dirp = readdir(dp)) != NULL) {
      files.push_back(std::string(dirp->d_name));
    }
    closedir(dp);
  }
  return files;
}

Inline uint32_t count_lines(std::string filename)
{
  InputFile file(filename);
  uint32_t line_count = 0;
  while(file.readNewLine() >= 0)
    line_count += 1;
  return (uint32_t) line_count;
}

template< typename T1, typename T2 >
std::vector< T2 > mapV(
	std::function< T2 (T1) > f,
	std::vector< T1 > elems
)
{
  std::vector< T2 > melems( elems.size() );
  std::transform(elems.begin(), elems.end(), melems.begin(), f);
  return melems;
}

template<typename R, typename A>
using Functor1 = R(A);

double intod(int i) {return (double) -1 * i;}

template< typename F, typename Ret, typename A, typename... Rest >
A arghelper(Ret (F::*)(A, Rest...) const);

template< typename F, typename Ret, typename A, typename... Rest >
Ret rethelper(Ret (F::*)(A, Rest...) const);

template< typename F >
struct FuncType{
  using ret = decltype( rethelper(&F::operator()) );
  using arg1 = decltype( arghelper(&F::operator()) ) ;
};


auto makeFunction(auto func) {
  using R = typename FuncType<decltype(func)>::ret;
  using A = typename FuncType<decltype(func)>::arg1;
  using Ftype = std::pair< R, A >;
  return Ftype {R(), A()};
}

template< typename A >
auto mapV2(auto func, std::vector<A> elems) //-> std::vector<decltype(func(A())>
{
  std::vector< decltype(func(A())) > melems(elems.size());
  std::transform(elems.begin(), elems.end(), melems.begin(), func);
  return melems;
}

template< typename R, typename A >
auto mapV3(Functor1< R, A > *func, std::vector<A> elems)
{
  std::vector< R > melems(elems.size());
  std::transform(elems.begin(), elems.end(), melems.begin(), func);
  return melems;
}


template< typename R, typename A  >
decltype(auto) mapV4(auto func, std::vector<A> elems)
{
  std::vector< R > melems(elems.size());
  std::transform(elems.begin(), elems.end(), melems.begin(), func);
  return melems;
}

//template< typename R, typename A >
template< typename T >
struct Function1;

template< typename R, typename A >
struct Function1<R(A)> {
  Function1(auto func) : apply(func) {}
  R operator()(const A& a ) {return apply(a);}
  std::vector<R> mapV(const std::vector<A>& elems) {
    std::vector<R> melems;
    std::transform(elems.begin(), elems.end(), melems.begin(), apply);
    return melems;
  }
  R (*apply)(const A&);
};

template< typename R, typename A>
struct MapV {
  MapV (auto func) : apply(func) {}
  std::vector< R > operator()(const std::vector< A >& elems){
    std::vector< R > melems;
    std::transform(elems.begin(), elems.end(), melems.begin(), apply);
    return melems;
  }
  R (*apply)(const A&);
};

template< typename A >
auto mapV5(auto func, std::vector<A> elems) //-> std::vector<decltype(func(A())>
{
  using R = typename FuncType<decltype(func)>::ret;
  std::vector< R > melems(elems.size());
  std::transform(elems.begin(), elems.end(), melems.begin(), func);
  return melems;
}

template< typename A >
auto mapV6(auto func, const std::vector< A >& elems) {
  using R = typename FuncType<decltype(func)>::ret;
  Function1< R(A) > f(func);
  return f.mapV(elems);
}

auto mapV7( auto func, auto elems) {
  using R = typename FuncType<decltype(func)>::ret;
  using A = typename FuncType<decltype(func)>::arg1;
  Function1<R(A)> f(func);
  return f.mapV(elems);
}

template< typename R, typename A >
std::vector< R > mapV8(Function1<R(A)> func, std::vector< A > elems) {
  std::vector< R > melems(elems.size());
  std::transform(elems.begin(), elems.end(), melems.begin(), func);
  return melems;
}


std::pair<uint8_t, uint32_t> find_chunk(
	const std::vector<uint8_t>::const_iterator itbegin,
	const std::vector<uint8_t>::const_iterator itend,
	uint8_t c
)
{
  uint8_t d =  (*itbegin == (uint8_t) 0) ? (uint8_t) 1 : (uint8_t) 0;
  std::vector<uint8_t>::const_iterator found = std::find(itbegin, itend, d);
  return std::make_pair(c, (uint32_t) std::distance(itbegin, found));
}

std::vector<std::pair<uint8_t, uint32_t> > makeChunks(
	std::vector<uint8_t>::const_iterator itbegin,
	std::vector<uint8_t>::const_iterator itend
)
{
  std::cout << "make chunks from vector of length "
						<< std::distance(itbegin, itend)
						<< std::endl;
  if (itbegin == itend) {
    return std::vector<std::pair<uint8_t, uint32_t> >();
  } else {
    uint8_t c =  (uint8_t) *itbegin;
    std::cout << "chunk for " << (uint32_t) c << std::endl;
    std::pair<uint8_t, uint32_t> p = find_chunk(itbegin, itend, c);
    std::cout << "found a chunk of " << (uint32_t) c
							<< " of length " << p.second
							<< std::endl;
    std::vector<std::pair<uint8_t, uint32_t> > cs  = makeChunks(
			itbegin + p.second, itend
		);
    cs.push_back(p);
    return cs;
  }

}

#if 0
//tests for code above, move it to tests
TEST_CASE ("Functional infrastructor works as intended", "[FunctionalInfrastructure]") {
  const std::vector<int> someints{-2, -1, 0, 1, 2};
  auto lfunc = [] (const int& i) { return (double) -1 * i;};
  std::vector<double> dfromi2 = mapV2(lfunc, someints);
  FuncType<decltype(lfunc)>::ret x = 1.0;
  FuncType<decltype(lfunc)>::arg1 y = 10;

  auto dfromiLambda = [] (const int& i) { return (double) -1 * i;};
  auto dfromi4 = mapV4<double>([] (const int& i) { return (double) -1 * i;}, someints);
  auto dfromi5 = mapV5(dfromiLambda,  someints);
  auto dfromif = Function1<double(int)>(dfromiLambda).mapV(someints);
  auto dfromi6 = mapV6(dfromiLambda, someints);
  // auto dfromi7 = mapV7(dfromiLambda, someints); //does not compile
  auto dfromi8 = mapV8(Function1<double(int)>(dfromiLambda), someints);
  //MapV<double, int> itod([] (const int& i) { return (double) -1 * i;});
  //REQUIRE( dfromi[1] == -2.0);
  //REQUIRE( dfromi[0] == 0.0);
}
#endif

