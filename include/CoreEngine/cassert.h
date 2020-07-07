#pragma once

#include <iostream>

#ifndef NDEBUG
#define cassert(Expr, Msg) \
	if(!(Expr)) \
	{ \
		std::cerr << "Assert failed:\t" << Msg << "\n" \
				  << "Condition:\t" << #Expr << "\n" \
				  << "Source:\t\t" << __FILE__ << ", line " << __LINE__ << "\n"; \
				  abort(); \
	}
#else
#define cassert(Expr, Msg);
#endif
