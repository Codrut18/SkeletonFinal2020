#include <iostream>
#include <functional>

int normalFunction(const int arg1, const bool arg2, const int initial)
{
	if (arg2)
		return arg1 * 2 + initial;
	else return initial - arg1;
}

class Doubling
{

public:
	Doubling(const int initial) : m_Initial(initial) {}

	//functor
	int operator () (const int arg1, const bool arg2)
	{
		if (arg2)
			return arg1 * 2 + m_Initial;
		else return m_Initial - arg1;
	}

private:
	const int m_Initial;
};

int advanced(const int arg1, const bool arg2, std::function<int(const int, const bool)> f)
{
	if (arg1 < 10)
		return f(arg1 * 2, arg2);
	else return f(arg1, arg2);
}

//int main()
//{
//	const int initialDollars = 10;
//
//	std::cout << "OG: " << normalFunction(2, true, initialDollars) << std::endl;
//	std::cout << "OG: " << normalFunction(10, false, initialDollars) << std::endl;
//
//	Doubling obj(initialDollars);
//	std::cout << "Functor: " << obj(2,true) << std::endl;
//	std::cout << "Functor: " << obj(10,false) << std::endl;
//
//	auto lambda = [&initialDollars](const int arg1, const bool arg2) -> int
//	{
//		if (arg2)
//			return arg1 * 2 + initialDollars;
//		else return initialDollars - arg1;
//	};
//	
//	auto lambdaTriple = [&initialDollars](const int arg1, const bool arg2) -> int
//	{
//		if (arg2)
//			return arg1 * 3 + initialDollars;
//		else return initialDollars - arg1;
//	};
//
//	std::cout << "Lambda: " << lambda(2, true) << std::endl;
//	std::cout << "Lambda: " << lambda(10, false) << std::endl;
//
//
//	getchar();
//	return 0;
//}