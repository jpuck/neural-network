// This file is licensed under the CC0 license (See http://creativecommons.org/publicdomain/zero/1.0/).
// And just to make sure you get the idea, it is also licensed under the WTFPL (See http://en.wikipedia.org/wiki/WTFPL).

#include "error.h"
#include <exception>
#include <signal.h>
#include <iostream>
#include <sstream>
#ifdef WINDOWS
	#include <windows.h>
	#include <float.h>
#else
	#define _GNU_SOURCE 1
	#include <fenv.h>
	#include <signal.h>
#endif

using std::exception;
using std::cerr;


const char* Ex::what() const throw()
{ 
	return m_message.c_str();
}

void enableFloatingPointExceptions()
{
#ifdef WINDOWS
	unsigned int cw = _control87(0, 0) & MCW_EM; // should we use _controlfp instead?
	cw &= ~(_EM_INVALID | _EM_ZERODIVIDE | _EM_OVERFLOW);
	_control87(cw,MCW_EM);
#else
#	ifdef DARWIN
	// todo: Anyone know how to do this on Darwin?
#	else
	feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
#	endif
#endif
}
